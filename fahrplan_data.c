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
},{										// DAY 2, SAAL 1
	DAY2 | SAAL1,11,30,12,30,
	"File -> Print -> Electronics",
	"A new circuit board printer will",
	"liberate you from the Arduino-",
	"Industrial Complex",
	"",
	"Jeff Gough",
	1,
	LANG_ENG | EV_LECTURE | TR_MAKING
},{										
	DAY2 | SAAL1,18,30,19,30,
	"The Concert",
	"a disconcerting moment for free",
	"culture",
	"",
	"",
	"Corey Cerovsek, Julien Quentin",
	1,
	LANG_ENG | EV_OTHER | TR_CULTURE
},{										
	DAY2 | SAAL1,20,30,21,30,
	"High-speed high-security",
	"cryptography:",
	"encrypting and authenticating",
	"the whole Internet",
	"",
	"Daniel J. Bernstein",
	2,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY2 | SAAL1,23,00,00,00,
	"The importance of resisting",
	"Excessive Government",
	"Surveillance",
	"Join me in exposing and chal-",
	"lenging the constant violations",
	"of our right to privacy",
	3,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{										
	DAY2 | SAAL1,00,15,01,15,
	"Stanislaw Lem",
	"Der enttäuschte Weltverbesserer",
	"Ein audiovisuelles Live-Feature",
    "A.Krolikowski, C.Kurz,",
	"I.Kwasniewski, J.-M. Loebel",
	"K.Kittler und M. Richter",
	2,
	LANG_GER | EV_LECTURE | TR_CULTURE
},{										// DAY 2, SAAL 2
	DAY2 | SAAL2,11,30,12,30,
	"Distributed FPGA Number",
	"Crunching For The Masses",
	"How we obtained the equivalent",
	"power of a Deep Crack for a",
	"fistful of dollars - and how...",
    "Felix Domke",
	2,
	LANG_GER | EV_LECTURE | TR_HACKING
},{	
	DAY2 | SAAL2,13,00,13,30,
	"Lying To The Neighbours",
	"Fiese Effekte mit tracker-less",
	"BitTorrent",
	"",
	"",
    "Astro",
	1,
	LANG_GER | EV_LECTURE | TR_HACKING
},{	
	DAY2 | SAAL2,13,45,14,15,
	"Node.js as a networking tool",
	"",
	"",
	"",
	"",
    "Felix Geisendoerfer",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{	
	DAY2 | SAAL2,14,30,15,00,
	"Logikschaltungen ohne Elektronik",
	"logische Schaltungen mit",
	"Pneumatik",
	"",
	"",
    "Aepex",
	1,
	LANG_GER | EV_LECTURE | TR_MAKING
},{	
	DAY2 | SAAL2,16,00,17,00,
	"Is the SSLiverse a safe place?",
	"An update on EFF's SSL",
	"Observatory project",
	"",
	"",
    "",
	1,
	LANG_ENG | EV_LECTURE | TR_COMMUNITY
},{	
	DAY2 | SAAL2,17,15,18,15,
	"Defense is not dead",
	"Why we will have more secure",
	"computers - tomorrow",
	"",
	"",
    "Andreas Bogk",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{	
	DAY2 | SAAL2,18,30,19,30,
	"Literarischer Abend",
	"",
	"",
	"",
	"",
    "Andreas Lehner, lars",
	1,
	LANG_GER | EV_PODIUM | TR_COMMUNITY
},{	
	DAY2 | SAAL2,21,45,22,45,
	"Adventures in Mapping",
	"Afghanistan Elections",
	"The story of 3 Ushahidi mapping",
	"and reporting projects.",
	"",
    "",
	2,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{	
	DAY2 | SAAL2,23,00,00,00,
	"Secure communications below the",
	"hearing threshold",
	"Improved approaches for auditive",
	"steganography",
	"",
    "Marcus Nutzinger, Rainer Poisel",
	2,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										//DAY 2, SAAL 3
	DAY2 | SAAL3,11,30,12,30,
	"Netzmedienrecht, Lobbyismus und",
	"Korruption",
	"Wie wirkt die Lobby von",
	"Medienkonzernen?",
	"",
    "Thomas Barth",
	2,
	LANG_GER | EV_WORKSHOP | TR_SOCIETY
},{										
	DAY2 | SAAL3,12,45,13,30,
	"Lightning Talks - Day 2",
	"Korruption",
	"4 minutes of fame",
	"",
	"",
    "Sven Guckes",
	1,
	LANG_ENG | EV_LTALK | TR_COMMUNITY
},{										
	DAY2 | SAAL3,14,00,15,00,
	"I Control Your Code",
	"Attack Vectors Through the Eyes",
	"of Software-based Fault",
	"Isolation",
	"",
    "Mathias Payer",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY2 | SAAL3,16,00,17,00,
	"A short political history of",
	"acoustics",
	"For whom, and to do what, the",
	"science of sound was developed",
	"in the 17th century",
    "Oona Leganovic",
	2,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{										
	DAY2 | SAAL3,17,15,18,15,
	"Data Analysis in Terabit",
	"Ethernet Traffic",
	"Solutions for monitoring and",
	"lawful interception within a lot",
	"of bits",
    "Lars Weiler",
	2,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY2 | SAAL3,18,30,19,30,
	"Techniken zur Identifizierung",
	"von Netzwerk-Protokollen",
	"",
	"",
	"",
    "Florian Adamsky",
	2,
	LANG_GER | EV_LECTURE | TR_SCIENCE
},{										
	DAY2 | SAAL3,21,45,22,45,
	"Console Hacking 2010",
	"Playstation 3, Xbox 360, Wii, ",
	"DSi",
	"",
	"",
    "bushing, marcan, sven",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										//DAY 3, SAAL 1
	DAY3 | SAAL1,11,30,13,30,
	"CCC-Jahresrueckblick 2010",
	"",
	"",
    "",
	"A.Bogk, A.Mueller-Maguhn,",
	"C.Kurz, F.Rieger",
	1,
	LANG_GER | EV_PODIUM | TR_COMMUNITY
},{										
	DAY3 | SAAL1,17,15,19,15,
	"Netzneutralitaet und QoS -",
	"ein Widerspruch?",
	"Fakten auf den Tisch",
    "",
	"",
	"Aandreas Bogk, Markus Beckedahl",
	2,
	LANG_GER | EV_PODIUM | TR_SOCIETY
},{										
	DAY3 | SAAL1,20,30,21,30,
	"Chip and PIN is Broken",
	"Vulnerabilities in the",
	"EMV Protocol",
    "",
	"",
	"Steven J. Murdoch",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY3 | SAAL1,21,45,22,45,
	"Fnord-Jahresrueckblick 2010",
	"von Atomausstieg bis",
	"Zwangsintegration",
    "",
	"",
	"Felix von Leitner, Frank Rieger",
	1,
	LANG_GER | EV_LECTURE | TR_SOCIETY
},{										
	DAY3 | SAAL1,23,00,01,00,
	"Hacker Jeopardy",
	"Number guessing for geeks",
	"",
    "",
	"",
	"Ray, Stefan 'Sec' Zehl",
	1,
	LANG_GER | EV_CONTEST | TR_COMMUNITY
},{										// DAY 3, SAAL 2
	DAY3 | SAAL2,11,30,12,30,
	"A Critical Overview of 10 years",
	"of Privacy Enhancing",
	"Technologies",
    "",
	"",
	"seda",
	3,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{										
	DAY3 | SAAL2,13,00,13,30,
	"AllColoursAreBeautiful",
	"interactive light installation",
	"inspired by blinkenlights",
    "",
	"",
	"Franz Pletz, lilafisch",
	1,
	LANG_GER | EV_LECTURE | TR_COMMUNITY
},{										
	DAY3 | SAAL2,14,30,15,00,
	"SIP home gateways under fire",
	"Source routing attacks applied",
	"to SIP",
    "",
	"",
	"Wolfgang Beck",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY3 | SAAL2,16,00,17,00,
	"DIY synthesizers and sound",
	"generators",
	"Where does the sound come from?",
    "",
	"",
	"syso",
	2,
	LANG_ENG | EV_LECTURE | TR_MAKING
},{										
	DAY3 | SAAL2,18,30,19,30,
	"INDECT -",
	"an EU-Surveillance Project",
	"",
    "",
	"",
	"Sylvia Johnigk",
	2,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{										
	DAY3 | SAAL2,20,30,21,30,
	"Digitale Spaltung per Gesetz",
	"Das Internet und geschaffene",
	"soziale Ungleichheit im Alltag",
    "von Erwerbslosen",
	"Betje Schwarz, Doris Gerbig,",
	"Kathrin Englert",
	1,
	LANG_GER | EV_LECTURE | TR_SOCIETY
},{										
	DAY3 | SAAL2,21,45,22,45,
	"Terrorists Win - Exploiting",
	"Telecommunications Data",
	"Retention?",
    "",
	"Kay Hamacher",
	"Stefan Katzenbeisser",
	3,
	LANG_ENG | EV_LECTURE | TR_SCIENCE
},{										
	DAY3 | SAAL2,23,00,00,00,
	"FrozenCache",
	"Mitigating cold-boot attacks for",
	"Full-Disk-Encryption software",
    "",
	"",
	"Juergen Pabel",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										// DAY 3, SAAL 3
	DAY3 | SAAL3, 12,45,13,45,
	"Lightning Talks - Tag 3",
	"where is my community?",
	"",
    "",
	"",
	"Sven Guckes",
	1,
	LANG_ENG | EV_LTALK | TR_COMMUNITY
},{										
	DAY3 | SAAL3, 14,00,15,00,
	"Building the ZombieGotcha",
	"How to eat friends and",
	"influence people.",
    "",
	"",
	"Eli Skipp, Travis Goodspeed",
	1,
	LANG_ENG | EV_LECTURE | TR_MAKING
},{										
	DAY3 | SAAL3, 16,00,17,00,
	"Safety on the Open Sea",
	"Safe navigation with the aid of",
	"an open sea chart.",
    "",
	"",
	"",
	1,
	LANG_ENG | EV_LECTURE | TR_SCIENCE
},{										
	DAY3 | SAAL3, 17,15,18,15,
	"Android geolocation using",
	"GSM network",
	"\"Where was Waldroid?\"",
    "",
	"",
	"Renaud Lifchitz",
	2,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY3 | SAAL3,18,30,19,30,
	"Your Infrastructure Will",
	"Kill You",
	"",
    "",
	"",
	"",
	2,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{										
	DAY3 | SAAL3,23,00,00,00,
	"\"Spoilers, Reverse Green,",
	"DECEL!\" or \"What's it doing",
	"now?\"",
    "Thoughts on the Automation and",
	"its Human Interfaces on",
	"Airplanes | Bernd Sieker",
	3,
	LANG_ENG | EV_LECTURE | TR_SCIENCE
},{										// DAY 4, SAAL 1
	DAY4 | SAAL1,11,30,12,30,
	"OMG WTF PDF",
	"What you didn't know about",
	"Acrobat",
    "",
	"",
	"Julia Wolf",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY4 | SAAL1,12,45,13,45,
	"Ich sehe nicht, dass wir nicht",
	"zustimmen werden",
	"Die Sprache des politischen",
    "Verrats und seiner Rechtferti-",
	"gung",
	"maha/Martin Haase",
	2,
	LANG_GER | EV_LECTURE | TR_SOCIETY
},{										
	DAY4 | SAAL1,14,00,15,00,
	"Three jobs that journalists",
	"will do in 2050",
	"Why future media may be more",
    "powerful (and more subversive)",
	"than ever before",
	"Annalee Newitz",
	2,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{										
	DAY4 | SAAL1,16,00,17,00,
	"How the internet sees you",
	"demonstrating what activities",
	"most ISPs see you doing on the",
    "internet",
	"",
	"",
	1,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{										
	DAY4 | SAAL1,17,15,18,15,
	"Security Nightmares",
	"",
	"",
    "",
	"",
	"Frank Rieger, Ron",
	1,
	LANG_GER | EV_LECTURE | TR_COMMUNITY
},{										
	DAY4 | SAAL1,18,30,19,30,
	"Abschlussveranstaltung",
	"",
	"",
    "",
	"",
	"",
	1,
	LANG_ENG | EV_LECTURE | TR_COMMUNITY
},{										// DAY 4, SAAL 2
	DAY4 | SAAL2,11,30,12,30,
	"Tor is Peace, Software Freedom",
	"is Slavery, Wikipedia is Truth",
	"The political philosophy of the",
    "Internet",
	"",
	"Adam",
	2,
	LANG_ENG | EV_LECTURE | TR_SOCIETY
},{										
	DAY4 | SAAL2,13,00,13,30,
	"Cybernetics for the Masses",
	"implants, sensory extension and",
	"silicon - all for you!",
    "",
	"",
	"",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY4 | SAAL2,13,45,14,15,
	"Hackers and Computer Science",
	"",
	"",
    "",
	"",
	"Sergey Bratus",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY4 | SAAL2,14,30,15,00,
	"A framework for automated",
	"architecture-independent",
	"gadget search",
    "CCC edition",
	"",
	"kornau",
	3,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										
	DAY4 | SAAL2,17,15,18,00,
	"News Key Recovery Attacks on",
	"RC4/WEP",
	"",
    "",
	"",
	"Martin Vuagnoux",
	2,
	LANG_ENG | EV_LECTURE | TR_HACKING
},{										// DAY 4, SAAL 3
	DAY4 | SAAL3,11,30,12,30,
	"A hacker's view on motor",
	"control and robotics",
	"",
    "",
	"",
	"",
	2,
	LANG_ENG | EV_LECTURE | TR_MAKING
},{										
	DAY4 | SAAL3,12,45,13,45,
	"Lightning Talks - Day 4",
	"where is my community?",
	"",
    "",
	"",
	"Sven Guckes",
	1,
	LANG_ENG | EV_LTALK | TR_COMMUNITY
},{										
	DAY4 | SAAL3,17,15,18,00,
	"Having fun with RTP",
	"\"Who is speaking???\"",
	"",
    "",
	"",
	"kapejod",
	1,
	LANG_ENG | EV_LECTURE | TR_HACKING
}

,{0xFF,0,0,0,0,"","","","","","",0,0} // end marker
};






















