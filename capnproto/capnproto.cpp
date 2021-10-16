//MIT License
//
//Copyright (c) 2019 Mindaugas Vinkelis
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "monster.capnp.h"
#include <testing/test.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>

using namespace capnp;
using namespace capnp_test;

// =======================================================================================
class TestInitMessageBuilder: public MessageBuilder {
public:
  TestInitMessageBuilder(kj::ArrayPtr<SegmentInit> segments): MessageBuilder(segments) {}

  kj::ArrayPtr<word> allocateSegment(uint minimumSize) override {
    auto array = kj::heapArray<word>(minimumSize);
    memset(array.begin(), 0, array.asBytes().size());
    allocations.add(kj::mv(array));
    return allocations.back();
  }

  kj::Vector<kj::Array<word>> allocations;
};

class TestMessageBuilder: public MallocMessageBuilder {
  // A MessageBuilder that tries to allocate an exact number of total segments, by allocating
  // minimum-size segments until it reaches the number, then allocating one large segment to
  // finish.

public:
  explicit TestMessageBuilder(uint desiredSegmentCount)
      : MallocMessageBuilder(0, AllocationStrategy::FIXED_SIZE),
        desiredSegmentCount(desiredSegmentCount) {}

  kj::ArrayPtr<word> allocateSegment(uint minimumSize) override {
    if (desiredSegmentCount <= 1) {
      if (desiredSegmentCount < 1) {
        std::cout << "Allocated more segments than desired.";
      } else {
        --desiredSegmentCount;
      }
      return MallocMessageBuilder::allocateSegment(SUGGESTED_FIRST_SEGMENT_WORDS);
    } else {
      --desiredSegmentCount;
      return MallocMessageBuilder::allocateSegment(minimumSize);
    }
  }

private:
  uint desiredSegmentCount;
};

class CapnProtoArchiver : public ISerializerTest {
public:
	CapnProtoArchiver() : size_(0), buf_size_(0) { 
		buf_size_ = 1*1024*1024*sizeof(word);
		buf_ = (word*) malloc(buf_size_);
		memset(buf_, 0, buf_size_);
	}
	~CapnProtoArchiver() { free(buf_); }

    Buf serialize(const std::vector<MyTypes::Monster> &data) override {
	// case 1: use fixed buffer, malloced in construction fuction.
	//capnp::MallocMessageBuilder message(kj::arrayPtr<word>(buf_, 1));

	// case 2: use fixed buffer[2048].
	//capnp::MallocMessageBuilder message(buffer);

	// case 3: use default construction.	
	capnp::MallocMessageBuilder message;

        Monsters::Builder ms_b= message.getRoot<Monsters>();
	auto monsters = ms_b.initMonsters(data.size());

        for (size_t i = 0; i < data.size(); i++) {
		Monster::Builder m_b = monsters[i];
		const MyTypes::Monster& data_ = data[i];

		Vec3::Builder vec3_ = m_b.initPos();
		vec3_.setX(data_.pos.x);
		vec3_.setY(data_.pos.y);
		vec3_.setZ(data_.pos.z);
	
		m_b.setMana(data_.mana);

		m_b.setHp(data_.hp);

		m_b.setName(data_.name);

/*	
		std::string inv{};
		m_b.initInventory(data_.inventory.size());
  		std::copy(data_.inventory.begin(), data_.inventory.end(), inv.begin());
		m_b.setInventory(kj::arrayPtr((const byte*)inv.data(), inv.size()));
*/

		::capnp::Data::Builder inv_ = m_b.initInventory(data_.inventory.size());
/*
		for (size_t j = 0; j < data_.inventory.size(); j++)
			inv_[j] = data_.inventory[j];
*/
		

		switch (data_.color) {
  		case MyTypes::Red: 
			m_b.setColor(Monster::Color::RED); break;
  		case MyTypes::Green: 
			m_b.setColor(Monster::Color::GREEN); break;
  		case MyTypes::Blue: 
			m_b.setColor(Monster::Color::BLUE); break;
  		}
	
		auto weapons_ = m_b.initWeapons(data_.weapons.size());
		for (size_t j = 0; j < data_.weapons.size(); j++)
		{
			Weapon::Builder w_b = weapons_[j];
			const MyTypes::Weapon& weapon_ = data_.weapons[j];
			w_b.setName(weapon_.name);
			w_b.setDamage(weapon_.damage);	
		}
			
			
		Weapon::Builder e_b = m_b.initEquipped();
		const MyTypes::Weapon& weapon_ = data_.equipped;
		e_b.setName(weapon_.name);
		e_b.setDamage(weapon_.damage);	

		
		auto paths_ = m_b.initPath(data_.path.size());
		for (size_t j = 0; j < data_.path.size(); j++)
		{
			Vec3::Builder v_b = paths_[j];
			const MyTypes::Vec3& vec3_ = data_.path[j];
			v_b.setX(vec3_.x);
			v_b.setY(vec3_.y);	
			v_b.setZ(vec3_.z);	
		}
			
        }

	array_ = message.getSegmentsForOutput();

	if (size_ == 0) 
	{
    	    for (auto segment : array_) {
        	size_t tmp_ = segment.asBytes().size();
		size_ += tmp_;
    	    }
		
	}

/*
		MessageBuilder::SegmentInit segInits_ = { kj::ArrayPtr<word>(buf_), array_[0].size() };
  		// Init a new builder from the old segments.
  		TestInitMessageBuilder message(kj::arrayPtr(&segInits_, 1));
*/

        return {
            reinterpret_cast<uint8_t *>(this->buf_),
            this->size_
        };
    }

    void deserialize(Buf buf, std::vector<MyTypes::Monster> &res) override {
        capnp::SegmentArrayMessageReader reader(this->array_);
        Monsters::Reader ms_b = reader.getRoot<Monsters>();
	auto monsters = ms_b.getMonsters();
	res.resize(monsters.size());

	for (size_t i = 0; i < monsters.size(); i++) {
                Monster::Reader m_b = monsters[i];
                MyTypes::Monster& data_ = res[i];

                Vec3::Reader vec3_ = m_b.getPos();
                data_.pos.x = vec3_.getX();
                data_.pos.y = vec3_.getY();
                data_.pos.z = vec3_.getZ();

                data_.mana = m_b.getMana();

                data_.hp = m_b.getHp();

                data_.name.assign(m_b.getName().cStr());
/*
		Data::Reader invs_ = m_b.getInventory();
		data_.inventory.resize(invs_.size());
		for (size_t j = 0; j < invs_.size(); j++)
			data_.inventory[j] = invs_[j];
*/	
		::capnp::Data::Reader invs_ = m_b.getInventory();
/*
		data_.inventory.resize(invs_.size());
		for (size_t j = 0; j < invs_.size(); j++)
			data_.inventory[j] = invs_[j];
*/
		
                switch (m_b.getColor()) {
                case Monster::Color::RED :
			data_.color = MyTypes::Red; break;
                case Monster::Color::GREEN:
			data_.color = MyTypes::Green; break;
                case Monster::Color::BLUE:
			data_.color = MyTypes::Blue; break;
                }

                auto weapons_ = m_b.getWeapons();
		data_.weapons.resize(weapons_.size());
                for (size_t j = 0; j < weapons_.size(); j++)
                {
                        Weapon::Reader w_b = weapons_[j];
			data_.weapons[j].name.assign(w_b.getName().cStr());
			data_.weapons[j].damage = w_b.getDamage();
                }


                Weapon::Reader e_b = m_b.getEquipped();
                data_.equipped.name.assign(e_b.getName().cStr());
                data_.equipped.damage = e_b.getDamage();


                auto paths_ = m_b.getPath();
		data_.path.resize(paths_.size());
                for (size_t j = 0; j < paths_.size(); j++)
                {
                        Vec3::Reader v_b = paths_[j];
                        data_.path[j].x = v_b.getX();
                        data_.path[j].y = v_b.getY();
                        data_.path[j].z = v_b.getZ();
		}
	}
    };

    TestInfo testInfo() const override {
        return {
            SerializationLibrary::CAPNPROTO,
            "general",
            ""
        };
    }

private:
  	kj::ArrayPtr<const kj::ArrayPtr<const capnp::word>> array_;
  	word* buf_;
	word buffer[2048];
	size_t size_;
	size_t buf_size_;
};

int main() {
    CapnProtoArchiver test;
    auto res = runTest(test);
    //google::protobuf::ShutdownProtobufLibrary();
    return res;
}
