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

void ErgebnisSpeichern(team* mannschaft, int anzahlTeams, int assists, std::string strIch);

int AssistsAuslesen(std::ifstream& file);

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
	
	file.open(".\\MeinName.txt", std::ios::in);
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
	std::cout<<"Versuche "<<argv[1]<<" zu oeffnen\n";
	file.open(argv[1], std::ios::in);
	
	if(!file.good())
	{
		std::cout<<"Klappt nicht\n";
		system("PAUSE");
		return EXIT_FAILURE;
	}
	std::cout<<argv[1]<<" geoeffnet\n";
	
	std::string suchBegriff;
	int anzahlTeams = AnzahlTeamsAuslesen(file);
	if(anzahlTeams == 0)
	{
		std::cout<<"Anzahl der Teams nicht gefunden => Abbruch\n";
		system("PAUSE");
		return EXIT_FAILURE;
	}
	std::cout<<"Anzahlteams ausgelesen\n";
	file.clear();
	file.seekg(0L, file.beg); //Zeiger an Anfang von file setzen

	team* mannschaften = new team[anzahlTeams];
	if(!TeamGroessenAuslesen(mannschaften, anzahlTeams, file))
	{
		std::cout<<"Auslesen der Teamgroesse fehlgeschlagen => Abbruch\n";
		system("PAUSE");
		return EXIT_FAILURE;
	}
	std::cout<<"Teamgroessen ausgelesen\n";
	file.clear();
	file.seekg(0L, file.beg); //Zeiger an Anfang von file setzen
	
	SpielerAuslesen(mannschaften, anzahlTeams, file);
	
	
	std::cout<<std::setw(32)<<"Name"<<" |";
	std::cout<<std::setw(12)<<"ID"<<" |";
	std::cout<<std::setw(5)<<"MMR"<<" |";
	std::cout<<std::setw(10)<<"downed me"<<" |";
	std::cout<<std::setw(10)<<"killed me"<<" |";
	std::cout<<std::setw(12)<<"downed mate"<<" |";
	std::cout<<std::setw(12)<<"killed mate"<<" |";
	std::cout<<std::setw(13)<<"downed by me"<<" |";
	std::cout<<std::setw(13)<<"killed by me"<<" |";
	std::cout<<std::setw(15)<<"downed by mate"<<" |";
	std::cout<<std::setw(15)<<"killed by mate"<<" |";
	std::cout<<std::setw(7)<<"Bounty"<<" |";
	std::cout<<std::setw(10)<<"Extracted"<<"\n";
	for(int team = 0; team < anzahlTeams; team++)
	{
		std::cout<<"Team "<<team<<":\n";
		TeamAusgeben(mannschaften[team]);
	}
	file.clear();
	file.seekg(0L, file.beg); //Zeiger an Anfang von file setzen
	
	int assists = AssistsAuslesen(file);
	std::cout<<"Erzielte Assists: "<<assists<<"\n\n";
	file.close();
	
	std::string antwort;
	std::cout<<"Ergebnis speichern? [j/n]: ";
	std::cin>>antwort;
	if(antwort.compare("j") == 0)
	{
		std::cout<<"Ergebnis wird gespeichert\n";
		ErgebnisSpeichern(mannschaften, anzahlTeams, assists, strIch);
	}
	else
	{
		std::cout<<"keine Speicherung\neinen schönen Tag noch\n";
	}

	delete []mannschaften;
	return 0;
}

int AssistsAuslesen(std::ifstream& file)
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
					suchBegriff = "<Attr name=\"MissionAccoladeEntry_";
					suchBegriff += std::to_string(lfdNr);
					NrGefunden = true;
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
						std::cout<<"Assists: "<<anzahlAssists<<" ("<<subZeile<<")\n";
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

void SpielerAuslesen(team* mannschaften, int anzahlTeams, std::ifstream& file)
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
	for(int i = 0; i < mannschaft.teamGroesse; i++)
	{
		SpielerAusgeben(mannschaft.teamMitglieder[i]);
	}
	return;
}

void SpielerAusgeben(player spieler)
{
	std::cout<<std::setw(32)<<spieler.name;
	std::cout<<" |"<<std::setw(12)<<spieler.profileID;
	std::cout<<" |"<<std::setw(5)<<spieler.mmr;
	std::cout<<" |"<<std::setw(10);
	if(spieler.downedMe != 0)
	{
		std::cout<<spieler.downedMe;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" |"<<std::setw(10);
	if(spieler.killedMe != 0)
	{
		std::cout<<spieler.killedMe;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" |"<<std::setw(12);
	if(spieler.downedMate != 0)
	{
		std::cout<<spieler.downedMate;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" |"<<std::setw(12);
	if(spieler.killedMate != 0)
	{
		std::cout<<spieler.killedMate;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" |"<<std::setw(13);
	if(spieler.downedByMe != 0)
	{
		std::cout<<spieler.downedByMe;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" |"<<std::setw(13);
	if(spieler.killedByMe != 0)
	{
		std::cout<<spieler.killedByMe;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" |"<<std::setw(15);
	if(spieler.downedByMate != 0)
	{
		std::cout<<spieler.downedByMate;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" |"<<std::setw(15);
	if(spieler.killedByMate != 0)
	{
		std::cout<<spieler.killedByMate;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" |"<<std::setw(7);
	if(spieler.bountyExtracted != 0)
	{
		std::cout<<spieler.bountyExtracted;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" |"<<std::setw(10);
	if(spieler.teamextraction != 0)
	{
		std::cout<<spieler.teamextraction<<"\n";
	}else
	{
		std::cout<<" "<<"\n";
	}
	return;
}

void ErgebnisSpeichern(team* mannschaft, int anzahlTeams, int assists, std::string strIch)
{
	std::ofstream ausgabe("./huntStatistik.csv", std::ios::out|std::ios::app|std::ios::ate);
	if(ausgabe.tellp() == ausgabe.beg)
	{
		ausgabe<<"Ich; MMR; Kills; Tode; Assists; Partner 1; MMR; Partner 2;";
		ausgabe<<"MMR; Extraction; Bounty; Kills (Team); Tode (Team); Gegner|MMR;";
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
	ausgabe<<aktSpieler.name<<";"<<aktSpieler.mmr<<";"<<myKills<<";"<<myDeaths<<";"<<assists<<";";
	
	for(int spieler = 0; spieler < 3; spieler++)
	{
		if(spieler == ichNr)continue;
		if(spieler >= mannschaft[meinTeam].teamGroesse)
		{
			ausgabe<<";;";
		}
		else
		{
			aktSpieler = mannschaft[meinTeam].teamMitglieder[spieler];
			ausgabe<<aktSpieler.name<<";"<<aktSpieler.mmr<<";";			
			extraction += aktSpieler.teamextraction;
			bounty += aktSpieler.bountyExtracted;
		}
	}
	
	ausgabe<<extraction<<";"<<bounty<<";"<<teamKills<<";"<<teamDeaths<<";";
	for(int team = 0; team < anzahlTeams; team++)
	{
		if(team == meinTeam)continue;
		for(int spieler = 0; spieler < mannschaft[team].teamGroesse; spieler++)
		{
			aktSpieler = mannschaft[team].teamMitglieder[spieler];
			ausgabe<<aktSpieler.name<<";"<<aktSpieler.mmr<<";";
		}
	}
	ausgabe<<"\n";
	ausgabe.close();
	return;
}