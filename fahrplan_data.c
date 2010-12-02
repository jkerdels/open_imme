#include <stdint.h>
#include "fahrplan_data.h"

__code const char langStr[2][8] = {
	"english",
	"german"
};

__code const char eventStr[6][9] = {
	"lecture",
	"podium",
	"contest",
	"ltngtalk",
	"workshop",
	"other"
};

__code const char trackStr[6][10] = {
	"community",
	"culture",
	"hacking",
	"making",
	"science",
	"society"
};

__code const FPItem fpData[] = {
// day 1, saal 1
{
	DAY1 | SAAL1,11,30,12,30,
	"27C3 Keynote",
    "We come in Peace",
	"",
	"",
	"",
	"",
	1,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{
	DAY1 | SAAL1,12,45,13,45,
	"Copyright Enforcement Vs.",
	"Freedoms",
    "ACTA, IPRED3 and other upcoming",
    "battles of the crusade against",
    "sharing",
	"Jeremie Zimmermann",
	2,
	LANG_ENG | EV_LECTURE | TR_SOCIETY	
},{
	DAY1 | SAAL1,14,00,15,00,
	"Von Zensursula über Censilia",
	"hin zum Kindernet",
	"Jahresrückblick rund um Internet",
	"Sperren, Sendezeitbegrenzungen",
	"im Internet und vermeintlichen",
	"Jugendschutz | Alvar C.H.Freude",
	2,
	LANG_GER | EV_LECTURE | TR_SOCIETY	
},{
	DAY1 | SAAL1,16,00,17,00,
	"Whistleblowing",
	"Licht ins Dunkel!",
	"",
	"",
	"",
	"",
	1,
	LANG_GER | EV_LECTURE | TR_SOCIETY
},{
	DAY1 | SAAL1,20,30,21,30,
	"Data Retention in the EU five",
    "years after the Directive",
	"Why the time is now to get",
	"active",
	"Katarzyna Szymielewicz, Patrick",
	"Breyer, Ralf Bendrath",
	2,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{
	DAY1 | SAAL1,00,15,01,15,
	"Pentanews Game Show",
	"Your opponents will be riddled",
	"as well",
	"",
	"",
	"Alien8, Astro",
	1,
	LANG_ENG | EV_OTHER | TR_COMMUNITY	
},{											// DAY 1, SAAL 2
	DAY1 | SAAL2,12,45,13,45,
	"Code deobfuscation by",
	"optimization",
	"",
	"",
	"",
	"Branko Spasojevic",
	2,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{
	DAY1 | SAAL2,14,00,15,00,
	"Contemporary Profiling of Web",
	"Users",
	"On Using Anonymizers and Still",
	"Get Fucked",
	"",
	"Dominik Herrmann, lexi",
	2,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{
	DAY1 | SAAL2,16,00,17,00,
	"Eins, zwei, drei - alle sind",
	"dabei",
	"Von der Volkszählung zum Bundes-",
	"melderegister",
	"",
	"Oliver \"Unicorn\" Knapp",
	2,
	LANG_GER | EV_LECTURE | TR_SOCIETY
},{
	DAY1 | SAAL2,17,15,18,15,
	"Friede sei mit Euren Daten",
	"Ein datenschutzrechtlicher Aus-",
	"flug in ein kirchliches ",
	"Paralleluniversum -",
	"",
	"Jochim Selzer",
	1,
	LANG_GER | EV_LECTURE | TR_SOCIETY
},{
	DAY1 | SAAL2,18,30,19,30,
	"hacking smart phones",
	"expanding the attack surface and",
	"then some",
	"",
	"",
	"",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{
	DAY1 | SAAL2,20,30,21,30,
	"Desktop on the Linux...",
	"(and BSD, of course)",
	"you're doing it confused? weird?",
    "strange? wrong?",
	"",
	"",
	2,
	LANG_ENG | EV_LECTURE | TR_COMMUNITY	
},{
	DAY1 | SAAL2,21,45,22,45,
	"Recent advances in IPv6",
	"insecurities",
	"",
	"",
	"",
	"vanHauser",
	2,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{
	DAY1 | SAAL2,23,00,00,00,
	"Spinning the electronic",
	"Wheel",
	"Still the bicycles for the 21th",
	"century",
	"",
	"",
	2,
	LANG_GER | EV_LECTURE | TR_MAKING
},{											// DAY 1, SAAL 3
	DAY1 | SAAL3,12,45,13,45,
	"From robot to robot",
	"Restoring creativity in school",
	"pupils using robotics",
	"",
	"",
	"",
	1,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{
	DAY1 | SAAL3,14,00,15,00,
	"JTAG/Serial/FLASH/PCB Embedded",
	"Reverse Engineering Tools and",
	"Techniques",
	"a dump of simple tools for em-",
	"bedded analysis at many layers",
	"",
	3,
	LANG_ENG | EV_LECTURE | TR_HACKING	
},{
	DAY1 | SAAL3,16,00,17,00,
	"Automatic Identification of",
	"Cryptographic Primitives in",
	"Software",
	"",
	"",
	"Felix Groebert",
	3,
	LANG_ENG | EV_LECTURE | TR_HACKING	
},{
	DAY1 | SAAL3,17,15,18,15,
	"USB and libusb for fun and",
	"profit",
	"USB: So much more than a serial",
	"port with power",
	"",
	"Peter Stuge",
	2,
	LANG_ENG | EV_LECTURE | TR_MAKING
},{
	DAY1 | SAAL3,21,45,22,45,
	"Cognitive Psychology for",
	"Hackers",
	"Bugs, exploits, and occasional",
	"patches",
	"",
	"Sai",
	2,
	LANG_ENG | EV_LECTURE | TR_SCIENCE
}







,{0xFF,0,0,0,0,"","","","","","",0,0} // end marker
};






















