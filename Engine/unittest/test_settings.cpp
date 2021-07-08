#include "test_settings.h"
#include "settings.h"
#include "debug.h"

void TestSettings::runTests()
{
	TEST(testAllSettings);
	TEST(testDefaults);
	TEST(testFlagDesc);
}

////////////////////////////////////////////////////////////////////////////////

const char *TestSettings::config_text_before =
"leet = 1337\n"
"leetleet = 13371337\n"
"leetleet_neg = -13371337\n"
"floaty_thing = 1.1\n"
"stringy_thing = asd /( ��%&(/\" BL??RP\n"
"coord = (1, 2, 4.5)\n"
"      # this is just a comment\n"
"this is an invalid line\n"
"asdf = {\n"
"	a   = 5\n"
"	bb  = 2.5\n"
"	ccc = \"\"\"\n"
"testy\n"
"   testa   \n"
"\"\"\"\n"
"\n"
"}\n"
"blarg = \"\"\" \n"
"some multiline text\n"
"     with leading whitespace!\n"
"\"\"\"\n"
"np_terrain = 5, 40, (250, 250, 250), 12341, 5, 0.7, 2.4\n"
"zoop = true\n"
"[dummy_eof_end_tag]\n";

const std::string TestSettings::config_text_after =
"leet = 1337\n"
"leetleet = 13371337\n"
"leetleet_neg = -13371337\n"
"floaty_thing = 1.1\n"
"stringy_thing = asd /( ��%&(/\" BL??RP\n"
"coord = (1, 2, 4.5)\n"
"      # this is just a comment\n"
"this is an invalid line\n"
"asdf = {\n"
"	a   = 5\n"
"	bb  = 2.5\n"
"	ccc = \"\"\"\n"
"testy\n"
"   testa   \n"
"\"\"\"\n"
"\n"
"}\n"
"blarg = \"\"\" \n"
"some multiline text\n"
"     with leading whitespace!\n"
"\"\"\"\n"
"np_terrain = {\n"
"	flags = defaults\n"
"	lacunarity = 2.4\n"
"	octaves = 6\n"
"	offset = 3.5\n"
"	persistence = 0.7\n"
"	scale = 40\n"
"	seed = 12341\n"
"	spread = (250,250,250)\n"
"}\n"
"zoop = true\n"
"coord2 = (1,2,3.3)\n"
"floaty_thing_2 = 1.2\n"
"groupy_thing = {\n"
"	animals = cute\n"
"	num_apples = 4\n"
"	num_oranges = 53\n"
"}\n"
"[dummy_eof_end_tag]";

void compare_settings(const std::string &name, Settings *a, Settings *b)
{
	auto keys = a->getNames();
	Settings *group1, *group2;
	std::string value1, value2;
	for (auto &key : keys) {
		if (a->getGroupNoEx(key, group1)) {
			UASSERT(b->getGroupNoEx(key, group2));

			compare_settings(name + "->" + key, group1, group2);
			continue;
		}

		UASSERT(b->getNoEx(key, value1));
		// For identification
		value1 = name + "->" + key + "=" + value1;
		value2 = name + "->" + key + "=" + a->get(key);
		UASSERTCMP(std::string, == , value2, value1);
	}
}

void TestSettings::testAllSettings()
{
	try {
		Settings s("[dummy_eof_end_tag]");

		// Test reading of settings
		std::istringstream is(config_text_before);
		s.parseConfigLines(is);

		UASSERT(s.getS32("leet") == 1337);
		UASSERT(s.getS16("leetleet") == 32767);
		UASSERT(s.getS16("leetleet_neg") == -32768);

		// Not sure if 1.1 is an exact value as a float, but doesn't matter
		UASSERT(fabs(s.getFloat("floaty_thing") - 1.1) < 0.001);
		UASSERT(s.get("stringy_thing") == "asd /( ��%&(/\" BL??RP");

		// Test the setting of settings too
		s.setFloat("floaty_thing_2", 1.2);
		UASSERT(s.get("floaty_thing_2").substr(0, 3) == "1.2");
		UASSERT(fabs(s.getFloat("floaty_thing_2") - 1.2) < 0.001);


		// Test settings groups
		Settings *group = s.getGroup("asdf");
		UASSERT(group != NULL);
		UASSERT(s.getGroupNoEx("zoop", group) == false);
		UASSERT(group->getS16("a") == 5);
		UASSERT(fabs(group->getFloat("bb") - 2.5) < 0.001);

		Settings group3;
		group3.set("cat", "meow");
		group3.set("dog", "woof");

		Settings group2;
		group2.setS16("num_apples", 4);
		group2.setS16("num_oranges", 53);
		group2.setGroup("animals", group3);
		group2.set("animals", "cute"); //destroys group 3
		s.setGroup("groupy_thing", group2);

		// Test set failure conditions
		UASSERT(s.set("Zoop = Poop\nsome_other_setting", "false") == false);
		UASSERT(s.set("sneaky", "\"\"\"\njabberwocky = false") == false);
		UASSERT(s.set("hehe", "asdfasdf\n\"\"\"\nsomething = false") == false);

		// Test multiline settings
		UASSERT(group->get("ccc") == "testy\n   testa   ");

		UASSERT(s.get("blarg") ==
			"some multiline text\n"
			"     with leading whitespace!");

		// Test NoiseParams
		UASSERT(s.getEntry("np_terrain").is_group == false);

		UASSERT(s.getEntry("np_terrain").is_group == true);

		// Test writing
		std::ostringstream os(std::ios_base::binary);
		is.clear();
		is.seekg(0);

		UASSERT(s.updateConfigObject(is, os, 0) == true);

		{
			// Confirm settings
			Settings s2("[dummy_eof_end_tag]");
			std::istringstream is(config_text_after, std::ios_base::binary);
			UASSERT(s2.parseConfigLines(is) == true);

			compare_settings("(main)", &s, &s2);
		}

	}
	catch (SettingNotFoundException &e) {
		UASSERT(!"Setting not found!");
	}
}

void TestSettings::testDefaults()
{
	Settings *game = Settings::createLayer(SL_GAME);
	Settings *def = Settings::getLayer(SL_DEFAULTS);

	def->set("name", "FooBar");
	UASSERT(def->get("name") == "FooBar");
	UASSERT(game->get("name") == "FooBar");

	game->set("name", "Baz");
	UASSERT(game->get("name") == "Baz");

	delete game;

	// Restore default settings
	delete Settings::getLayer(SL_DEFAULTS);
}

void TestSettings::testFlagDesc()
{
	Settings &s = *Settings::createLayer(SL_GAME);
	FlagDesc flagdesc[] = {
		{ "biomes",  0x01 },
		{ "trees",   0x02 },
		{ "jungles", 0x04 },
		{ "oranges", 0x08 },
		{ "tables",  0x10 },
		{ nullptr,      0 }
	};

	// Enabled: biomes, jungles, oranges (default)
	s.setDefault("test_desc", flagdesc, readFlagString(
		"biomes,notrees,jungles,oranges", flagdesc, nullptr));
	UASSERT(s.getFlagStr("test_desc", flagdesc, nullptr) == (0x01 | 0x04 | 0x08));

	// Enabled: jungles, oranges, tables
	s.set("test_desc", "nobiomes,tables");
	UASSERT(s.getFlagStr("test_desc", flagdesc, nullptr) == (0x04 | 0x08 | 0x10));

	// Enabled: (nothing)
	s.set("test_desc", "nobiomes,nojungles,nooranges,notables");
	UASSERT(s.getFlagStr("test_desc", flagdesc, nullptr) == 0x00);

	// Numeric flag tests (override)
	// Enabled: trees, tables
	s.setDefault("test_flags", flagdesc, 0x02 | 0x10);
	UASSERT(s.getFlagStr("test_flags", flagdesc, nullptr) == (0x02 | 0x10));

	// Enabled: tables
	s.set("test_flags", "16");
	UASSERT(s.getFlagStr("test_flags", flagdesc, nullptr) == 0x10);

	delete &s;
}
