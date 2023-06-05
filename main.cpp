#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <vector>
#include <iomanip>

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

void TeamAusgeben(team mannschaft);
void SpielerAusgeben(player spieler);

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
	{
		std::cout<<"Team "<<team<<":\n";
		TeamAusgeben(mannschaften[team]);
	}		
	
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
				SpielerBountyLesen(team, spieler, Zeile, mannschaften);
				SpielerDownedByMeLesen(team, spieler, Zeile, mannschaften);
				SpielerDownedByTMLesen(team, spieler, Zeile, mannschaften);
				SpielerDownedMeLesen(team, spieler, Zeile, mannschaften);
				SpielerDownedTMLesen(team, spieler, Zeile, mannschaften);
				SpielerKilledByMeLesen(team, spieler, Zeile, mannschaften);
				SpielerKilledByTMLesen(team, spieler, Zeile, mannschaften);
				SpielerKilledMeLesen(team, spieler, Zeile, mannschaften);
				SpielerKilledTMLesen(team, spieler, Zeile, mannschaften);
				SpielerMMRLesen(team, spieler, Zeile, mannschaften);
				SpielerProfilIDLesen(team, spieler, Zeile, mannschaften);
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

void TeamAusgeben(team mannschaft)
{
	std::cout<<std::setw(32)<<"Name";
	std::cout<<std::setw(16)<<"ID";
	std::cout<<std::setw(10)<<"MMR";
	std::cout<<std::setw(10)<<"Bounty";
	std::cout<<std::setw(16)<<"downed me";
	std::cout<<std::setw(16)<<"killed me";
	std::cout<<std::setw(16)<<"downed mate";
	std::cout<<std::setw(16)<<"killed mate";
	std::cout<<std::setw(16)<<"downed by me";
	std::cout<<std::setw(16)<<"killed by me";
	std::cout<<std::setw(16)<<"downed by mate";
	std::cout<<std::setw(16)<<"killed by mate";
	std::cout<<std::setw(16)<<"Extracted"<<"\n";
	for(int i = 0; i < mannschaft.teamGroesse; i++)
	{
		SpielerAusgeben(mannschaft.teamMitglieder[i]);
	}
	return;
}

void SpielerAusgeben(player spieler)
{
	std::cout<<std::setw(32)<<spieler.name;
	std::cout<<std::setw(16)<<spieler.profileID;
	std::cout<<std::setw(10)<<spieler.mmr;
	std::cout<<std::setw(10)<<spieler.bountyExtracted;
	std::cout<<std::setw(16)<<spieler.downedMe;
	std::cout<<std::setw(16)<<spieler.killedMe;
	std::cout<<std::setw(16)<<spieler.downedMate;
	std::cout<<std::setw(16)<<spieler.killedMate;
	std::cout<<std::setw(16)<<spieler.downedByMe;
	std::cout<<std::setw(16)<<spieler.killedByMe;
	std::cout<<std::setw(16)<<spieler.downedByMate;
	std::cout<<std::setw(16)<<spieler.killedByMate;
	std::cout<<std::setw(16)<<spieler.teamextraction<<"\n";
	return;
}