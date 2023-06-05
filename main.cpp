#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <vector>

typedef struct{
	std::string name;
	int profileID;
	int mmr;
	int bountyExtracted;
	int downedMe;
	int killedMe;
	int downedMate;
	int killedMate;
	int downedByMe;
	int KilledByMe;
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

int AnzahlTeamsAuslesen(std::ifstream& file);
bool TeamGroessenAuslesen(team* mannschaften, int anzahlTeams, std::ifstream& file);
void SpielerAuslesen(team* mannschaften, int anzahlTeams, std::ifstream& file);
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

int main(int argc, char** argv)
{
	std::ifstream file;
	if(argc == 1)
	{
		std::cout<<"Keine Datei zum Auswerten angegeben\n";
		system("PAUSE");
		return 1;
	}
	else
	{
		std::cout<<"Versuche "<<argv[1]<<" zu oeffnen\n";
		file.open(argv[1], std::ifstream::in);
	}
	
	if(!file.good())
	{
		std::cout<<"Klappt nicht\n";
		return 1;
	}
	std::cout<<"Erfolg!!!\n";
	
	std::string suchBegriff;
	int anzahlTeams = AnzahlTeamsAuslesen(file);
	if(anzahlTeams == 0)
	{
		std::cout<<"Anzahl der Teams nicht gefunden => Abbruch\n";
		return EXIT_FAILURE;
	}
	file.seekg(0, file.beg); //Zeiger an Anfang von file setzen
	std::cout<<"position Dateizeiger: "<<file.tellg()<<"\n";

	team* mannschaften = new team[anzahlTeams];
	if(!TeamGroessenAuslesen(mannschaften, anzahlTeams, file))
	{
		std::cout<<"Auslesen der Teamgroesse fehlgeschlagen => Abbruch\n";
		return EXIT_FAILURE;
	}
	for(int i = 0; i < anzahlTeams; i++)
	{
		std::cout<<i<<". Team: "<<mannschaften[i].teamGroesse<<" Mann stark\n";
	}
	file.close();
	
	file.open(argv[1], std::ifstream::in);
		if(!file.good())
	{
		std::cout<<"Klappt nicht\n";
		return 1;
	}
	
	SpielerAuslesen(mannschaften, anzahlTeams, file);
	for(int team = 0; team < anzahlTeams; team++)
		for(int spieler = 0; spieler < mannschaften[team].teamGroesse; spieler++)
			std::cout<<"Spieler "<<team<<" - "<<spieler<<": "<<mannschaften[team].teamMitglieder[spieler].name<<"\n";
	
	system("PAUSE");
	file.close();

	delete []mannschaften;
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
			std::cout<<suchBegriff<<" gefunden\n";
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
				std::cout<<suchBegriff<<" gefunden\n";
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

void SpielerAuslesen(team* mannschaften, int anzahlTeams, std::ifstream& file)
{
	char leseZeile[256];
	std::string Zeile;
	std::string subZeile;
	std::string suchBegriff;

	while(!file.eof())
	{
		file.getline(leseZeile, 256);
		Zeile = leseZeile;
		for(int team=0; team < anzahlTeams; team++)
		{
			for(int spieler=0; spieler < mannschaften[team].teamGroesse; spieler++)
			{
				SpielerNameLesen(team, spieler, Zeile, mannschaften);
				SpielerExtractionLesen(team, spieler, Zeile, mannschaften);
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
		std::cout<<suchBegriff<<" gefunden\n";
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
		std::cout<<suchBegriff<<" gefunden\n";
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
{}
void SpielerDownedByMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}
void SpielerDownedByTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}
void SpielerDownedMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}
void SpielerDownedTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}
void SpielerKilledByMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}
void SpielerKilledByTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}
void SpielerKilledMeLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}
void SpielerKilledTMLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}
void SpielerMMRLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}
void SpielerProfilIDLesen(int teamNr, int spielerNr, std::string& Zeile, team* mannschaften)
{}

	
/*	suchBegriffe[1] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_teamextraction\" value=\"";
	suchBegriffe[2] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_bountyextracted\" value=\"";
	suchBegriffe[3] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_downedbyme\" value=\"";
	suchBegriffe[4] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_downedbyteammate\" value=\"";
	suchBegriffe[5] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_downedme\" value=\"";
	suchBegriffe[6] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_downedteammate\" value=\"";
	suchBegriffe[7] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_killedbyme\" value=\"";
	suchBegriffe[8] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_killedbyteammate\" value=\"";
	suchBegriffe[9] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_killedme\" value=\"";
	suchBegriffe[10] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_killedteammate\" value=\"";
	suchBegriffe[11] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_mmr\" value=\"";
	suchBegriffe[12] = " <Attr name=\"MissionBagPlayer_" + teamNr + "_" + spielerNr + "_profileid\" value=\"";
	
	return;
}*/