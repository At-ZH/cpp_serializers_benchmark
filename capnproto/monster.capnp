@0xae93ad7f8553edc5;

$import "/capnp/c++.capnp".namespace("capnp_test");

struct Record {
    ids @0 :List(Int64);
    strings @1 :List(Text);
}


struct Vec3 {
	x @0 :Float32;
	y @1 :Float32;
	z @2 :Float32;
}

struct Weapon {
	name @0 :Text;
	damage @1 :Int32;
}

struct Monster {
	pos 		@0 :Vec3;
	mana 		@1 :Int32;
	hp 		@2 :Int32;
	name 		@3 :Text;
	inventory 	@4 :Data; 
	enum Color {
        	red 	@0;
        	green 	@1;
        	blue 	@2;
  	}
  	color 		@5 :Color;
  	weapons 	@6 :List(Weapon);
  	equipped 	@7 :Weapon;
	path 		@8 :List(Vec3);
}

struct Monsters {
    monsters @0 :List(Monster);
	}
