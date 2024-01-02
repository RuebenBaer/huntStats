#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <vector>
#include <iomanip>
#include <filesystem>

typedef struct{
	std::string name;
	std::string profileID;
	int mmr;
	int bountyExtracted;
	int downedMe;
	int killedMe;
	int downedMate;
	int killedMate;
	int downedByMe;
	int killedByMe;
	int downedByMate;
	int killedByMate;
	bool teamextraction;
}player;

typedef struct{
	player teamMitglieder[3];
	int teamGroesse;
	int mmr;
	int numPlayers;
	bool ownTeam;
}team;

typedef struct{
	team Mannschaften[12];
	int anzahlTeams;
	int assists;
	int eigeneMMR;
}match;

match SpielAuslesen(char* DateiName, std::string strIch);

int AnzahlAuszeichnungenAuslesen(std::ifstream& file);
int AnzahlTeamsAuslesen(std::ifstream& file);
bool TeamGroessenAuslesen(team* mannschaften, int anzahlTeams, std::ifstream& file);
void SpielerAuslesen(match& aktSpiel, std::ifstream& file, std::string strIch);
void SpielerNameLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerExtractionLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerBountyLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerDownedByMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerDownedByTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerDownedMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerDownedTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerKilledByMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerKilledByTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerKilledMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerKilledTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerMMRLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);
void SpielerProfilIDLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften);

void BildschirmAusgabe(team* mannschaften, int anzahlTeams);
void TeamAusgeben(team mannschaft);
void SpielerAusgeben(player spieler);

bool DateiAusgabe(team* mannschaften, int anzahlTeams, int assists, int dMMR);
void TeamAusgebenDatei(team mannschaft, std::ofstream& ausgabe);
void SpielerAusgebenDatei(player spieler, std::ofstream& ausgabe);

std::string FindeNeuenNamen(void);
void ErgebnisSpeichern(team* mannschaft, int anzahlTeams, int assists, std::string strIch);

int AssistsAuslesen(int anzahlAuszeichnungen, std::ifstream& file);
int MMRSchaetzen(match& aktSpiel);

int DeltaMMR(int meineMMR, int gegnerMMR, bool eigenerTod)
{	
	float MMR_Teiler = 400;
	float MMR_Fkt = 33;
	int s = eigenerTod ? 0 : 1;
	
	int dMMR = MMR_Fkt * (s - (1/(1+pow(10, (gegnerMMR - meineMMR) / MMR_Teiler)))) + 0.5;
	
	return dMMR;
}

int main(int argc, char** argv)
{
	std::ifstream file;
	std::string strIch;
	if(argc == 1)
	{
		std::cout<<"Keine Datei zum Auswerten angegeben\n";
		system("PAUSE");
		return 1;
	}
	
	file.open(".\\huntstats.cfg", std::ios::in);
	char leseZeile[256];
	if(file.good())
	{
		file.getline(leseZeile, 256);
		strIch = leseZeile;
		file.close();
		std::cout<<"Mein Name: "<<strIch<<"\n\n";
	}else
	{
		std::cout<<argv[0]<<" => Name kann nicht gelesen werden\n";
		system("PAUSE");
		return EXIT_FAILURE;
	}

	bool speichern = true;
	/*std::string antwort;
	std::cout<<"Ergebnis(se) speichern? [j/n]: ";
	std::cin>>antwort;
	if((antwort.compare("j") == 0)||(antwort.compare("J") == 0))
	{
		std::cout<<"Ergebnis wird gespeichert\n";
		speichern = true;
	}
	else
	{
		std::cout<<"Keine Speicherung der Ergebnisse\n";
		speichern = false;
	}*/
	
	std::string archivName = FindeNeuenNamen();
	if(archivName.empty())
	{
		std::cout<<"Kein Dateiname verfu:gbar\n";
		system("PAUSE");
		return 0;
	}
	std::ifstream  src(argv[1], std::ios::binary);
	std::ofstream  dst(archivName,   std::ios::binary);
	if(src.is_open() && dst.is_open())
	{
		dst << src.rdbuf();
	}else{
		std::cout<<"Datei "<<argv[1]<<" nicht kopiert\n";
		system("PAUSE");
		return 0;
	}

	match aktSpiel;
	for(int i = 1; i < argc;i++)
	{
		aktSpiel = SpielAuslesen(argv[i], strIch);
		if(speichern) ErgebnisSpeichern(aktSpiel.Mannschaften, aktSpiel.anzahlTeams, aktSpiel.assists, strIch);
	}
	
	system("PAUSE");
	return 0;
}

std::string FindeNeuenNamen(void)
{
	std::ifstream test;
	char testName[256];
	for(int i = 1; i < 50000; i++)
	{
		sprintf(testName, "./Attributes/Eingelesen/attributes%d.xml", i);
		test.open(testName);
		if(test.is_open())
		{
			test.close();
		}
		else
		{
			std::cout<<testName<<" ist nicht vergeben\n";
			return std::string(testName);
		}
	}
	return std::string();
}

match SpielAuslesen(char* DateiName, std::string strIch)
{
	match aktSpiel;
	std::ifstream file;
	
	std::cout<<"Versuche "<<DateiName<<" zu oeffnen...";
	file.open(DateiName, std::ios::in);
	
	if(!file.good())
	{
		std::cout<<"Klappt nicht\n";
		system("PAUSE");
		return aktSpiel;
	}
	std::cout<<"geoeffnet\n";

	int anzahlAuszeichnungen = AnzahlAuszeichnungenAuslesen(file);
	//std::cout<<anzahlAuszeichnungen<<" Auszeichnungen vergeben\n";
	file.clear();
	file.seekg(0L, file.beg); //Zeiger an Anfang von file setzen
	
	aktSpiel.anzahlTeams = AnzahlTeamsAuslesen(file);
	if(aktSpiel.anzahlTeams == 0)
	{
		std::cout<<"Anzahl der Teams nicht gefunden => Abbruch\n";
		system("PAUSE");
		return aktSpiel;
	}
	file.clear();
	file.seekg(0L, file.beg); //Zeiger an Anfang von file setzen

	if(!TeamGroessenAuslesen(aktSpiel.Mannschaften, aktSpiel.anzahlTeams, file))
	{
		std::cout<<"Auslesen der Teamgroesse fehlgeschlagen => Abbruch\n";
		system("PAUSE");
		return aktSpiel;
	}
	file.clear();
	file.seekg(0L, file.beg); //Zeiger an Anfang von file setzen
	
	SpielerAuslesen(aktSpiel, file, strIch);
	file.clear();
	file.seekg(0L, file.beg); //Zeiger an Anfang von file setzen
	
	int dMMR = MMRSchaetzen(aktSpiel);
	
	BildschirmAusgabe(aktSpiel.Mannschaften, aktSpiel.anzahlTeams);
	
	aktSpiel.assists = AssistsAuslesen(anzahlAuszeichnungen, file);
	std::cout<<"Erzielte Assists: "<<aktSpiel.assists<<"\n";
	std::cout<<"Geschaetzte neue MMR: "<<dMMR<<"\n\n";
	
	DateiAusgabe(aktSpiel.Mannschaften, aktSpiel.anzahlTeams, aktSpiel.assists, dMMR);
	
	file.close();

	return aktSpiel;
}

int AssistsAuslesen(int anzahlAuszeichnungen, std::ifstream& file)
{
	char leseZeile[256];
	int lfdNr, anzahlAssists;
	bool NrGefunden = false;
	std::string Zeile;
	std::string subZeile;
	std::string suchBegriff;
	std::size_t found, foundEnde;
	
	while(!file.eof())
	{
		file.getline(leseZeile, 256);
		Zeile = leseZeile;
		if(!NrGefunden)
		{
			suchBegriff = "_category\" value=\"accolade_players_killed_assist\"/>";
			foundEnde = Zeile.find(suchBegriff);
			if(foundEnde != std::string::npos)
			{
				found = Zeile.find('_');
				if(found != std::string::npos)
				{
					subZeile = Zeile.substr(found+1, foundEnde - found - 1);
					lfdNr = std::atoi(subZeile.c_str());
					if(lfdNr < anzahlAuszeichnungen)
					{
						suchBegriff = "<Attr name=\"MissionAccoladeEntry_";
						suchBegriff += std::to_string(lfdNr);
						NrGefunden = true;
					}
				}
			}
		}
		else
		{
			found = Zeile.find(suchBegriff);
			if(found != std::string::npos)
			{
				found = Zeile.find("_hits\" value=\"");
				if(found != std::string::npos)
				{
					found += std::string("_hits\" value=\"").length();
					foundEnde = Zeile.find('\"', found);
					if(foundEnde != std::string::npos)
					{
						subZeile = Zeile.substr(found, foundEnde - found);
						anzahlAssists = std::atoi(subZeile.c_str());
						return anzahlAssists;
					}
				}
			}
			else
			{
				NrGefunden = false;
			}
		}
	}
	return 0;
}

int AnzahlTeamsAuslesen(std::ifstream& file)
{
	char leseZeile[256];
	int anzahlTeams;
	std::string Zeile;
	std::string subZeile;
	std::string suchBegriff;
	std::size_t found, foundEnde;
	
	suchBegriff = " <Attr name=\"MissionBagNumTeams\" value=\"";
	while(!file.eof())
	{
		file.getline(leseZeile, 256);
		Zeile = leseZeile;
		found = Zeile.find(suchBegriff);
		if(found != std::string::npos)
		{
			found += suchBegriff.length();
			foundEnde = Zeile.find('\"', found);
			if(foundEnde != std::string::npos)
			{
				subZeile = Zeile.substr(found, foundEnde - found);
				anzahlTeams = std::atoi(subZeile.c_str());
				if(anzahlTeams > 0)return anzahlTeams;
				return 0;
			}
		}
	}
	return 0;
}

bool TeamGroessenAuslesen(team* mannschaften, int anzahlTeams, std::ifstream& file)
{
	char leseZeile[256];
	std::string Zeile;
	std::string subZeile;
	std::string suchBegriff;
	std::size_t found, foundEnde;
	
	int teamGefunden = anzahlTeams;
	
	while(!file.eof())
	{
		file.getline(leseZeile, 256);
		Zeile = leseZeile;
		
		for(int team=0; team < anzahlTeams; team++)
		{
			suchBegriff = " <Attr name=\"MissionBagTeam_";
			suchBegriff += std::to_string(team);
			suchBegriff += "_numplayers\" value=\"";
			found = Zeile.find(suchBegriff);
			if(found != std::string::npos)
			{
				found += suchBegriff.length();
				foundEnde = Zeile.find('\"', found);
				if(foundEnde != std::string::npos)
				{
					subZeile = Zeile.substr(found, foundEnde - found);
					mannschaften[team].teamGroesse = std::atoi(subZeile.c_str());
					teamGefunden--;
					break;
				}
			}
		}
	}
	return (!(teamGefunden));
}

int AnzahlAuszeichnungenAuslesen(std::ifstream& file)
{
	char leseZeile[256];
	int anzahlAuszeichnungen;
	std::string Zeile;
	std::string subZeile;
	std::string suchBegriff;
	std::size_t found, foundEnde;
	
	suchBegriff = " <Attr name=\"MissionBagNumAccolades\" value=\"";
	while(!file.eof())
	{
		file.getline(leseZeile, 256);
		Zeile = leseZeile;
		found = Zeile.find(suchBegriff);
		if(found != std::string::npos)
		{
			found += suchBegriff.length();
			foundEnde = Zeile.find('\"', found);
			if(foundEnde != std::string::npos)
			{
				subZeile = Zeile.substr(found, foundEnde - found);
				anzahlAuszeichnungen = std::atoi(subZeile.c_str());
				if(anzahlAuszeichnungen > 0)return anzahlAuszeichnungen;
				return 0;
			}
		}
	}
	return 0;
}

void SpielerAuslesen(match& aktSpiel, std::ifstream& file, std::string strIch)
{
	char leseZeile[256];
	std::string Zeile;
	std::string subZeile;
	std::string suchBegriff;
	
	std::cout<<"Beginne Spieler auszulesen\n";

	while(!file.eof())
	{
		file.getline(leseZeile, 256);
		Zeile = leseZeile;
		for(int team=0; team < aktSpiel.anzahlTeams; team++)
		{
			for(int spieler=0; spieler < aktSpiel.Mannschaften[team].teamGroesse; spieler++)
			{
				SpielerNameLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerExtractionLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerBountyLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerDownedByMeLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerDownedByTMLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerDownedMeLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerDownedTMLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerKilledByMeLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerKilledByTMLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerKilledMeLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerKilledTMLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerMMRLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				SpielerProfilIDLesen(team, spieler, Zeile, aktSpiel.Mannschaften);
				
				if(strIch.compare(aktSpiel.Mannschaften[team].teamMitglieder[spieler].name) == 0)
				{
					aktSpiel.eigeneMMR = aktSpiel.Mannschaften[team].teamMitglieder[spieler].mmr;
					aktSpiel.Mannschaften[team].ownTeam = true;
					//std::cout<<"Eigene MMR = "<<aktSpiel.eigeneMMR;
				}
			}
		}
	}
	return;
}

void SpielerNameLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{	
	std::string subZeile;
	std::size_t found, foundEnde;
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_blood_line_name\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].name = subZeile;
			return;
		}
	}
	return;
}

void SpielerExtractionLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{	
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_teamextraction\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].teamextraction = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerBountyLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_bountyextracted\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].bountyExtracted = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerDownedByMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_downedbyme\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].downedByMe = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerDownedByTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_downedbyteammate\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].downedByMate = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerDownedMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_downedme\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].downedMe = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerDownedTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_downedteammate\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].downedMate = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerKilledByMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_killedbyme\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].killedByMe = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerKilledByTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_killedbyteammate\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].killedByMate = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerKilledMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_killedme\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].killedMe = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerKilledTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_killedteammate\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].killedMate = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerMMRLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_mmr\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].mmr = std::atoi(subZeile.c_str());
			return;
		}
	}
	return;
}

void SpielerProfilIDLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{
	std::string subZeile;
	std::size_t found, foundEnde;
	
	std::string suchBegriff = " <Attr name=\"MissionBagPlayer_";
	suchBegriff += std::to_string(teamNr);
	suchBegriff += "_";
	suchBegriff += std::to_string(spielerNr);
	suchBegriff += "_profileid\" value=\"";

	found = Zeile.find(suchBegriff);
	if(found != std::string::npos)
	{
		found += suchBegriff.length();
		foundEnde = Zeile.find('\"', found);
		if(foundEnde != std::string::npos)
		{
			subZeile = Zeile.substr(found, foundEnde - found);
			mannschaften[teamNr].teamMitglieder[spielerNr].profileID = subZeile;
			return;
		}
	}
	return;
}

void BildschirmAusgabe(team* mannschaften, int anzahlTeams)
{
	std::cout<<std::setw(56)<<char(218)<<std::setfill(char(196))<<std::setw(20)<<char(194);
	std::cout<<std::setw(16)<<char(191)<<"\n";
	std::cout.fill(' ');
	std::cout<<std::setw(56)<<char(179)<<std::setw(19)<<"Kills       "<<char(179)<<std::setw(15)<<"Deaths    "<<char(179)<<"\n";
	std::cout.fill(char(196));
	std::cout<<char(218)<<std::setw(34)<<char(194)<<std::setw(14)<<char(194)<<std::setw(7)<<char(197);
	std::cout<<std::setw(10)<<char(194)<<std::setw(10)<<char(197);
	std::cout<<std::setw(8)<<char(194)<<std::setw(8)<<char(197);
	std::cout<<std::setw(10)<<char(194)<<std::setw(13)<<char(191)<<"\n";
	
	std::cout.fill(' ');
	
	std::cout<<char(179)<<std::setw(32)<<"Name"<<" "<<char(179);
	std::cout<<std::setw(12)<<"ID"<<" "<<char(179);
	std::cout<<std::setw(5)<<"MMR"<<" "<<char(179);
	std::cout<<std::setw(8)<<"by me "<<" "<<char(179);
	std::cout<<std::setw(8)<<"by mate"<<" "<<char(179);
	std::cout<<std::setw(6)<<"me "<<" "<<char(179);
	std::cout<<std::setw(6)<<"mate"<<" "<<char(179);
	std::cout<<std::setw(8)<<"Bounty"<<" "<<char(179);
	std::cout<<std::setw(11)<<"Extracted"<<" "<<char(179)<<"\n";
	for(int teamNr = 0; teamNr < anzahlTeams; teamNr++)
	{
		std::cout.fill(char(196));
		std::cout<<char(195)<<std::setw(34)<<char(197);
		std::cout<<std::setw(14)<<char(197);
		std::cout<<std::setw(7)<<char(197);
		std::cout<<std::setw(10)<<char(197);
		std::cout<<std::setw(10)<<char(197);
		std::cout<<std::setw(8)<<char(197);
		std::cout<<std::setw(8)<<char(197);
		std::cout<<std::setw(10)<<char(197);
		std::cout<<std::setw(13)<<char(180)<<"\n";
		std::cout.fill(' ');
		TeamAusgeben(mannschaften[teamNr]);
	}
	std::cout.fill(char(196));
	std::cout<<char(192)<<std::setw(34)<<char(193);
	std::cout<<std::setw(14)<<char(193);
	std::cout<<std::setw(7)<<char(193);
	std::cout<<std::setw(10)<<char(193);
	std::cout<<std::setw(10)<<char(193);
	std::cout<<std::setw(8)<<char(193);
	std::cout<<std::setw(8)<<char(193);
	std::cout<<std::setw(10)<<char(193);
	std::cout<<std::setw(13)<<char(217)<<"\n\n";
	std::cout.fill(' ');
}

void TeamAusgeben(team mannschaft)
{
	for(int i = 0; i < mannschaft.teamGroesse; i++)
	{
		SpielerAusgeben(mannschaft.teamMitglieder[i]);
	}
	return;
}

void SpielerAusgeben(player spieler)
{
	int killedByMe = spieler.downedByMe + spieler.killedByMe;
	int killedByMate = spieler.downedByMate + spieler.killedByMate;
	int killedMe = spieler.downedMe + spieler.killedMe;
	int killedMate = spieler.downedMate + spieler.killedMate;
	
	std::cout<<char(179)<<std::setw(32)<<spieler.name<<" "<<char(179);
	std::cout<<std::setw(12)<<spieler.profileID<<" "<<char(179);
	std::cout<<std::setw(5)<<spieler.mmr<<" "<<char(179);
	std::cout<<std::setw(8);
	if(killedByMe != 0)
	{
		std::cout<<killedByMe;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" "<<char(179)<<std::setw(8);
	if(killedByMate != 0)
	{
		std::cout<<killedByMate;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" "<<char(179)<<std::setw(6);
	if(killedMe != 0)
	{
		std::cout<<killedMe;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" "<<char(179)<<std::setw(6);
	if(killedMate != 0)
	{
		std::cout<<killedMate;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" "<<char(179)<<std::setw(8);
	if(spieler.bountyExtracted != 0)
	{
		std::cout<<spieler.bountyExtracted;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" "<<char(179)<<std::setw(11);
	if(spieler.teamextraction != 0)
	{
		std::cout<<spieler.teamextraction;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" "<<char(179)<<"\n";
	return;
}

void ErgebnisSpeichern(team* mannschaft, int anzahlTeams, int assists, std::string strIch)
{
	std::ofstream ausgabe("./huntStatistik.csv", std::ios::out|std::ios::app|std::ios::ate);
	if(ausgabe.tellp() == ausgabe.beg)
	{
		ausgabe<<"Ich,MMR;Kills,Tode;Assists,Partner 1,MMR,Partner 2,";
		ausgabe<<"MMR,Extraction,Bounty,Kills (Team),Tode (Team),Gegner-MMR ('+' = killed by me | '-' = killed me),\n";
	}
	int teamDeaths, teamKills;
	int myDeaths, myKills;
	int extraction, bounty;
	
	teamDeaths = teamKills = myDeaths = myKills = extraction = bounty = 0;
	
	//Eigenen Namen finden
	int meinTeam, ichNr;
	for(int team = 0; team < anzahlTeams; team++)
	{
		for(int spieler = 0; spieler < mannschaft[team].teamGroesse; spieler++)
		{
			if(strIch.compare(mannschaft[team].teamMitglieder[spieler].name) == 0)
			{
				ichNr = spieler;
				meinTeam = team;
			}
		}
	}
	
	player aktSpieler;
	for(int team = 0; team < anzahlTeams; team++)
	{
		if(team == meinTeam)continue;
		for(int spieler = 0; spieler < mannschaft[team].teamGroesse; spieler++)
		{
			aktSpieler = mannschaft[team].teamMitglieder[spieler];
			teamDeaths += aktSpieler.downedMe + aktSpieler.killedMe + aktSpieler.downedMate + aktSpieler.killedMate;
			myDeaths += aktSpieler.downedMe + aktSpieler.killedMe;
			teamKills += aktSpieler.downedByMe + aktSpieler.killedByMe + aktSpieler.downedByMate + aktSpieler.killedByMate;
			myKills += aktSpieler.downedByMe + aktSpieler.killedByMe;
		}
	}
	
	aktSpieler = mannschaft[meinTeam].teamMitglieder[ichNr];
	extraction += aktSpieler.teamextraction;
	bounty += aktSpieler.bountyExtracted;
	ausgabe<<aktSpieler.name<<","<<aktSpieler.mmr<<","<<myKills<<","<<myDeaths<<","<<assists<<",";
	
	for(int spieler = 0; spieler < 3; spieler++)
	{
		if(spieler == ichNr)continue;
		if(spieler >= mannschaft[meinTeam].teamGroesse)
		{
			ausgabe<<",,";
		}
		else
		{
			aktSpieler = mannschaft[meinTeam].teamMitglieder[spieler];
			ausgabe<<aktSpieler.name<<","<<aktSpieler.mmr<<",";			
			extraction += aktSpieler.teamextraction;
			bounty += aktSpieler.bountyExtracted;
		}
	}
	
	ausgabe<<extraction<<","<<bounty<<","<<teamKills<<","<<teamDeaths<<",";
	for(int team = 0; team < anzahlTeams; team++)
	{
		if(team == meinTeam)continue;
		for(int spieler = 0; spieler < mannschaft[team].teamGroesse; spieler++)
		{
			aktSpieler = mannschaft[team].teamMitglieder[spieler];
			int killCount = 0;
			while(killCount < (aktSpieler.downedByMe + aktSpieler.killedByMe))
			{
				ausgabe<<"+"<<aktSpieler.mmr<<",";
				killCount++;
			}
			killCount = 0;
			while(killCount < (aktSpieler.downedMe + aktSpieler.killedMe))
			{
				ausgabe<<"-"<<aktSpieler.mmr<<",";
				killCount++;
			}
		}
	}
	ausgabe<<"\n";
	ausgabe.close();
	return;
}

bool DateiAusgabe(team* mannschaften, int anzahlTeams, int assists, int dMMR)
{
	std::ofstream ausgabe;
	ausgabe.open("./BildschirmSpiegel.txt", std::ios::out|std::ios::app|std::ios::ate);
	if(!ausgabe.good())
	{
		std::cerr<<"Bildschirmspiegel konnte nicht geoeffnet werden\n";
		return false;
	}
	
	ausgabe<<std::setw(56)<< char(43) <<std::setfill(char(45))<<std::setw(20)<<char(43);
	ausgabe<<std::setw(16)<<char(43)<<"\n";
	ausgabe.fill(' ');
	ausgabe<<std::setw(56)<<char(124)<<std::setw(19)<<"Kills       "<<char(124)<<std::setw(15)<<"Deaths    "<<char(124)<<"\n";
	ausgabe.fill(char(45));
	ausgabe<<char(43)<<std::setw(34)<<char(43)<<std::setw(14)<<char(43)<<std::setw(7)<<char(43);
	ausgabe<<std::setw(10)<<char(43)<<std::setw(10)<<char(43);
	ausgabe<<std::setw(8)<<char(43)<<std::setw(8)<<char(43);
	ausgabe<<std::setw(10)<<char(43)<<std::setw(13)<<char(43)<<"\n";
	
	ausgabe.fill(' ');
	
	ausgabe<<char(124)<<std::setw(32)<<"Name"<<" "<<char(124);
	ausgabe<<std::setw(12)<<"ID"<<" "<<char(124);
	ausgabe<<std::setw(5)<<"MMR"<<" "<<char(124);
	ausgabe<<std::setw(8)<<"by me "<<" "<<char(124);
	ausgabe<<std::setw(8)<<"by mate"<<" "<<char(124);
	ausgabe<<std::setw(6)<<"me "<<" "<<char(124);
	ausgabe<<std::setw(6)<<"mate"<<" "<<char(124);
	ausgabe<<std::setw(8)<<"Bounty"<<" "<<char(124);
	ausgabe<<std::setw(11)<<"Extracted"<<" "<<char(124)<<"\n";
	for(int teamNr = 0; teamNr < anzahlTeams; teamNr++)
	{
		ausgabe.fill(char(45));
		ausgabe<<char(43)<<std::setw(34)<<char(43);
		ausgabe<<std::setw(14)<<char(43);
		ausgabe<<std::setw(7)<<char(43);
		ausgabe<<std::setw(10)<<char(43);
		ausgabe<<std::setw(10)<<char(43);
		ausgabe<<std::setw(8)<<char(43);
		ausgabe<<std::setw(8)<<char(43);
		ausgabe<<std::setw(10)<<char(43);
		ausgabe<<std::setw(13)<<char(43)<<"\n";
		ausgabe.fill(' ');
		TeamAusgebenDatei(mannschaften[teamNr], ausgabe);
	}
	ausgabe.fill(char(45));
	ausgabe<<char(43)<<std::setw(34)<<char(43);
	ausgabe<<std::setw(14)<<char(43);
	ausgabe<<std::setw(7)<<char(43);
	ausgabe<<std::setw(10)<<char(43);
	ausgabe<<std::setw(10)<<char(43);
	ausgabe<<std::setw(8)<<char(43);
	ausgabe<<std::setw(8)<<char(43);
	ausgabe<<std::setw(10)<<char(43);
	ausgabe<<std::setw(13)<<char(43)<<"\n\n";
	ausgabe.fill(' ');
	
	ausgabe<<"Assists: "<<assists<<"\n";
	ausgabe<<"Geschaetzte neue MMR: "<<dMMR<<"\n\n";
	
	return true;
}

void TeamAusgebenDatei(team mannschaft, std::ofstream& ausgabe)
{
	for(int i = 0; i < mannschaft.teamGroesse; i++)
	{
		SpielerAusgebenDatei(mannschaft.teamMitglieder[i], ausgabe);
	}
	return;
}

void SpielerAusgebenDatei(player spieler, std::ofstream& ausgabe)
{
	int killedByMe = spieler.downedByMe + spieler.killedByMe;
	int killedByMate = spieler.downedByMate + spieler.killedByMate;
	int killedMe = spieler.downedMe + spieler.killedMe;
	int killedMate = spieler.downedMate + spieler.killedMate;
	
	ausgabe<<char(124)<<std::setw(32)<<spieler.name<<" "<<char(124);
	ausgabe<<std::setw(12)<<spieler.profileID<<" "<<char(124);
	ausgabe<<std::setw(5)<<spieler.mmr<<" "<<char(124);
	ausgabe<<std::setw(8);
	if(killedByMe != 0)
	{
		ausgabe<<killedByMe;
	}else
	{
		ausgabe<<" ";
	}
	ausgabe<<" "<<char(124)<<std::setw(8);
	if(killedByMate != 0)
	{
		ausgabe<<killedByMate;
	}else
	{
		ausgabe<<" ";
	}
	ausgabe<<" "<<char(124)<<std::setw(6);
	if(killedMe != 0)
	{
		ausgabe<<killedMe;
	}else
	{
		ausgabe<<" ";
	}
	ausgabe<<" "<<char(124)<<std::setw(6);
	if(killedMate != 0)
	{
		ausgabe<<killedMate;
	}else
	{
		ausgabe<<" ";
	}
	ausgabe<<" "<<char(124)<<std::setw(8);
	if(spieler.bountyExtracted != 0)
	{
		ausgabe<<spieler.bountyExtracted;
	}else
	{
		ausgabe<<" ";
	}
	ausgabe<<" "<<char(124)<<std::setw(11);
	if(spieler.teamextraction != 0)
	{
		ausgabe<<spieler.teamextraction;
	}else
	{
		ausgabe<<" ";
	}
	ausgabe<<" "<<char(124)<<"\n";
	return;
}

int MMRSchaetzen(match& aktSpiel)
{
	int schaetzMMR = aktSpiel.eigeneMMR;
	for(int team=0; team < aktSpiel.anzahlTeams; team++)
	{
		if(aktSpiel.Mannschaften[team].ownTeam == true)continue;
		for(int spieler=0; spieler < aktSpiel.Mannschaften[team].teamGroesse; spieler++)
		{
			player aktPlayer = aktSpiel.Mannschaften[team].teamMitglieder[spieler];
			int killedByMe = aktPlayer.downedByMe + aktPlayer.killedByMe;
			int killedMe = aktPlayer.downedMe + aktPlayer.killedMe;
			schaetzMMR += (killedMe * DeltaMMR(aktSpiel.eigeneMMR, aktPlayer.mmr, true));
			schaetzMMR += (killedByMe * DeltaMMR(aktSpiel.eigeneMMR, aktPlayer.mmr, false));
		}
	}
	return schaetzMMR;
}