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
	player teamMembers[3];
	int teamSize;
	int mmr;
	int numPlayers;
	bool ownTeam;
}team;

typedef struct{
	team teamsInvolved[12];
	int numTeams;
	int assists;
	int ownMMR;
}match;

match ReadMatch(char* FileName, std::string strMe);

int ReadNumAccolades(std::ifstream& file);
int ReadNumTeams(std::ifstream& file);
bool ReadTeamSize(team* teamsInvolved, int numTeams, std::ifstream& file);
void ReadPlayer(match& curMatch, std::ifstream& file, std::string strMe);
void ReadPlayerName(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerExtraction(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerBounty(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerDownedByMe(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerDownedByTM(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerDownedMe(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerDownedTM(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerKilledByMe(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerKilledByTM(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerKilledMe(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerKilledTM(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerMMR(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);
void ReadPlayerProfilID(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved);

void PrintToScreen(team* teamsInvolved, int numTeams);
void PrintTeams(team curTeam);
void PrintPlayers(player curPlayer);

bool PrintToFile(team* teamsInvolved, int numTeams, int assists, int dMMR);
void PrintTeamsToFile(team curTeam, std::ofstream& output);
void PrintPlayersToFile(player curPlayer, std::ofstream& output);

void SaveResults(team* curTeam, int numTeams, int assists, std::string strMe);

int ReadAssists(int numAccolades, std::ifstream& file);
int EstimateMMR(match& curMatch);

int DeltaMMR(int myMMR, int opponentMMR, bool ownDeath)
{	
	float MMR_Divisor = 400;
	float MMR_Factor = 33;
	int s = ownDeath ? 0 : 1;
	
	int dMMR = MMR_Factor * (s - (1/(1+pow(10, (opponentMMR - myMMR) / MMR_Divisor)))) + 0.5;
	
	return dMMR;
}

int main(int argc, char** argv)
{
	std::ifstream file;
	std::string strMe;
	if(argc == 1)
	{
		std::cout<<"No input file to analyse given\n";
		system("PAUSE");
		return 1;
	}
	
	file.open("..\\..\\huntstats.cfg", std::ios::in);
	char readRow[256];
	if(file.good())
	{
		file.getline(readRow, 256);
		strMe = readRow;
		file.close();
		std::cout<<"My name: "<<strMe<<"\n\n";
	}else
	{
		std::cout<<argv[0]<<" => name cannot be read\n";
		system("PAUSE");
		return EXIT_FAILURE;
	}

	std::string answer;
	bool save;
	std::cout<<"Save result(s)? [y/n]: ";
	std::cin>>answer;
	if((answer.compare("y") == 0)||(answer.compare("Y") == 0))
	{
		std::cout<<"Result(s) will be saved\n";
		save = true;
	}
	else
	{
		std::cout<<"Result(s) will NOT be saved\n";
		save = false;
	}
	
	match curMatch;
	for(int i = 1; i < argc;i++)
	{
		curMatch = ReadMatch(argv[i], strMe);
		if(save) SaveResults(curMatch.teamsInvolved, curMatch.numTeams, curMatch.assists, strMe);
	}
	
	system("PAUSE");
	return 0;
}

match ReadMatch(char* FileName, std::string strMe)
{
	match curMatch;
	std::ifstream file;
	
	std::cout<<"Try to open "<<FileName<<"...";
	file.open(FileName, std::ios::in);
	
	if(!file.good())
	{
		std::cout<<"without success\n";
		system("PAUSE");
		return curMatch;
	}
	std::cout<<"opened\n";

	int numAccolades = ReadNumAccolades(file);
	file.clear();
	file.seekg(0L, file.beg); //set stream pointer to beginning of the file
	
	curMatch.numTeams = ReadNumTeams(file);
	if(curMatch.numTeams == 0)
	{
		std::cout<<"Number of teams not found => canceling\n";
		system("PAUSE");
		return curMatch;
	}
	file.clear();
	file.seekg(0L, file.beg); //set stream pointer to beginning of the file

	if(!ReadTeamSize(curMatch.teamsInvolved, curMatch.numTeams, file))
	{
		std::cout<<"Reading team size failed => canceling\n";
		system("PAUSE");
		return curMatch;
	}
	file.clear();
	file.seekg(0L, file.beg); //set stream pointer to beginning of the file
	
	ReadPlayer(curMatch, file, strMe);
	file.clear();
	file.seekg(0L, file.beg); //set stream pointer to beginning of the file
	
	int dMMR = EstimateMMR(curMatch);
	
	PrintToScreen(curMatch.teamsInvolved, curMatch.numTeams);
	
	curMatch.assists = ReadAssists(numAccolades, file);
	std::cout<<"Assists obtained: "<<curMatch.assists<<"\n";
	std::cout<<"Estimated new MMR: "<<dMMR<<"\n\n";
	
	PrintToFile(curMatch.teamsInvolved, curMatch.numTeams, curMatch.assists, dMMR);
	
	file.close();

	return curMatch;
}

int ReadAssists(int numAccolades, std::ifstream& file)
{
	char readRow[256];
	int curNmb, numAssists;
	bool nmbFound = false;
	std::string row;
	std::string subrow;
	std::string searchFor;
	std::size_t found, foundEnd;
	
	while(!file.eof())
	{
		file.getline(readRow, 256);
		row = readRow;
		if(!nmbFound)
		{
			searchFor = "_category\" value=\"accolade_players_killed_assist\"/>";
			foundEnd = row.find(searchFor);
			if(foundEnd != std::string::npos)
			{
				found = row.find('_');
				if(found != std::string::npos)
				{
					subrow = row.substr(found+1, foundEnd - found - 1);
					curNmb = std::atoi(subrow.c_str());
					if(curNmb < numAccolades)
					{
						searchFor = "<Attr name=\"MissionAccoladeEntry_";
						searchFor += std::to_string(curNmb);
						nmbFound = true;
					}
				}
			}
		}
		else
		{
			found = row.find(searchFor);
			if(found != std::string::npos)
			{
				found = row.find("_hits\" value=\"");
				if(found != std::string::npos)
				{
					found += std::string("_hits\" value=\"").length();
					foundEnd = row.find('\"', found);
					if(foundEnd != std::string::npos)
					{
						subrow = row.substr(found, foundEnd - found);
						numAssists = std::atoi(subrow.c_str());
						return numAssists;
					}
				}
			}
			else
			{
				nmbFound = false;
			}
		}
	}
	return 0;
}

int ReadNumTeams(std::ifstream& file)
{
	char readRow[256];
	int numTeams;
	std::string row;
	std::string subrow;
	std::string searchFor;
	std::size_t found, foundEnd;
	
	searchFor = " <Attr name=\"MissionBagNumTeams\" value=\"";
	while(!file.eof())
	{
		file.getline(readRow, 256);
		row = readRow;
		found = row.find(searchFor);
		if(found != std::string::npos)
		{
			found += searchFor.length();
			foundEnd = row.find('\"', found);
			if(foundEnd != std::string::npos)
			{
				subrow = row.substr(found, foundEnd - found);
				numTeams = std::atoi(subrow.c_str());
				if(numTeams > 0)return numTeams;
				return 0;
			}
		}
	}
	return 0;
}

bool ReadTeamSize(team* teamsInvolved, int numTeams, std::ifstream& file)
{
	char readRow[256];
	std::string row;
	std::string subrow;
	std::string searchFor;
	std::size_t found, foundEnd;
	
	int teamGefunden = numTeams;
	
	while(!file.eof())
	{
		file.getline(readRow, 256);
		row = readRow;
		
		for(int team=0; team < numTeams; team++)
		{
			searchFor = " <Attr name=\"MissionBagTeam_";
			searchFor += std::to_string(team);
			searchFor += "_numplayers\" value=\"";
			found = row.find(searchFor);
			if(found != std::string::npos)
			{
				found += searchFor.length();
				foundEnd = row.find('\"', found);
				if(foundEnd != std::string::npos)
				{
					subrow = row.substr(found, foundEnd - found);
					teamsInvolved[team].teamSize = std::atoi(subrow.c_str());
					teamGefunden--;
					break;
				}
			}
		}
	}
	return (!(teamGefunden));
}

int ReadNumAccolades(std::ifstream& file)
{
	char readRow[256];
	int numAccolades;
	std::string row;
	std::string subrow;
	std::string searchFor;
	std::size_t found, foundEnd;
	
	searchFor = " <Attr name=\"MissionBagNumAccolades\" value=\"";
	while(!file.eof())
	{
		file.getline(readRow, 256);
		row = readRow;
		found = row.find(searchFor);
		if(found != std::string::npos)
		{
			found += searchFor.length();
			foundEnd = row.find('\"', found);
			if(foundEnd != std::string::npos)
			{
				subrow = row.substr(found, foundEnd - found);
				numAccolades = std::atoi(subrow.c_str());
				if(numAccolades > 0)return numAccolades;
				return 0;
			}
		}
	}
	return 0;
}

void ReadPlayer(match& curMatch, std::ifstream& file, std::string strMe)
{
	char readRow[256];
	std::string row;
	std::string subrow;
	std::string searchFor;
	
	std::cout<<"Starting to read players\n";

	while(!file.eof())
	{
		file.getline(readRow, 256);
		row = readRow;
		for(int team=0; team < curMatch.numTeams; team++)
		{
			for(int curPlayer=0; curPlayer < curMatch.teamsInvolved[team].teamSize; curPlayer++)
			{
				ReadPlayerName(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerExtraction(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerBounty(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerDownedByMe(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerDownedByTM(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerDownedMe(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerDownedTM(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerKilledByMe(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerKilledByTM(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerKilledMe(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerKilledTM(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerMMR(team, curPlayer, row, curMatch.teamsInvolved);
				ReadPlayerProfilID(team, curPlayer, row, curMatch.teamsInvolved);
				
				if(strMe.compare(curMatch.teamsInvolved[team].teamMembers[curPlayer].name) == 0)
				{
					curMatch.ownMMR = curMatch.teamsInvolved[team].teamMembers[curPlayer].mmr;
					curMatch.teamsInvolved[team].ownTeam = true;
				}
			}
		}
	}
	return;
}

void ReadPlayerName(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{	
	std::string subrow;
	std::size_t found, foundEnd;
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_blood_line_name\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].name = subrow;
			return;
		}
	}
	return;
}

void ReadPlayerExtraction(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{	
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_teamextraction\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].teamextraction = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerBounty(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_bountyextracted\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].bountyExtracted = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerDownedByMe(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_downedbyme\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].downedByMe = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerDownedByTM(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_downedbyteammate\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].downedByMate = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerDownedMe(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_downedme\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].downedMe = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerDownedTM(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_downedteammate\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].downedMate = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerKilledByMe(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_killedbyme\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].killedByMe = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerKilledByTM(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_killedbyteammate\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].killedByMate = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerKilledMe(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_killedme\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].killedMe = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerKilledTM(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_killedteammate\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].killedMate = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerMMR(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_mmr\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].mmr = std::atoi(subrow.c_str());
			return;
		}
	}
	return;
}

void ReadPlayerProfilID(int teamNmb, int playerNmb, std::string& row, team* teamsInvolved)
{
	std::string subrow;
	std::size_t found, foundEnd;
	
	std::string searchFor = " <Attr name=\"MissionBagPlayer_";
	searchFor += std::to_string(teamNmb);
	searchFor += "_";
	searchFor += std::to_string(playerNmb);
	searchFor += "_profileid\" value=\"";

	found = row.find(searchFor);
	if(found != std::string::npos)
	{
		found += searchFor.length();
		foundEnd = row.find('\"', found);
		if(foundEnd != std::string::npos)
		{
			subrow = row.substr(found, foundEnd - found);
			teamsInvolved[teamNmb].teamMembers[playerNmb].profileID = subrow;
			return;
		}
	}
	return;
}

void PrintToScreen(team* teamsInvolved, int numTeams)
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
	for(int teamNmb = 0; teamNmb < numTeams; teamNmb++)
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
		PrintTeams(teamsInvolved[teamNmb]);
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

void PrintTeams(team curTeam)
{
	for(int i = 0; i < curTeam.teamSize; i++)
	{
		PrintPlayers(curTeam.teamMembers[i]);
	}
	return;
}

void PrintPlayers(player curPlayer)
{
	int killedByMe = curPlayer.downedByMe + curPlayer.killedByMe;
	int killedByMate = curPlayer.downedByMate + curPlayer.killedByMate;
	int killedMe = curPlayer.downedMe + curPlayer.killedMe;
	int killedMate = curPlayer.downedMate + curPlayer.killedMate;
	
	std::cout<<char(179)<<std::setw(32)<<curPlayer.name<<" "<<char(179);
	std::cout<<std::setw(12)<<curPlayer.profileID<<" "<<char(179);
	std::cout<<std::setw(5)<<curPlayer.mmr<<" "<<char(179);
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
	if(curPlayer.bountyExtracted != 0)
	{
		std::cout<<curPlayer.bountyExtracted;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" "<<char(179)<<std::setw(11);
	if(curPlayer.teamextraction != 0)
	{
		std::cout<<curPlayer.teamextraction;
	}else
	{
		std::cout<<" ";
	}
	std::cout<<" "<<char(179)<<"\n";
	return;
}

void SaveResults(team* curTeam, int numTeams, int assists, std::string strMe)
{
	std::ofstream output("./huntStatistik.csv", std::ios::out|std::ios::app|std::ios::ate);
	if(output.tellp() == output.beg)
	{
		output<<"Me,MMR;Kills,Deaths;Assists,Partner 1,MMR,Partner 2,";
		output<<"MMR,Extraction,Bounty,Kills (Team),Deaths (Team),Enemy-MMR ('+' = killed by me | '-' = killed me),\n";
	}
	int teamDeaths, teamKills;
	int myDeaths, myKills;
	int extraction, bounty;
	
	teamDeaths = teamKills = myDeaths = myKills = extraction = bounty = 0;
	
	//find own name
	int myTeam, myNmb;
	for(int team = 0; team < numTeams; team++)
	{
		for(int curPlayer = 0; curPlayer < curTeam[team].teamSize; curPlayer++)
		{
			if(strMe.compare(curTeam[team].teamMembers[curPlayer].name) == 0)
			{
				myNmb = curPlayer;
				myTeam = team;
			}
		}
	}
	
	player curMatcher;
	for(int team = 0; team < numTeams; team++)
	{
		if(team == myTeam)continue;
		for(int curPlayer = 0; curPlayer < curTeam[team].teamSize; curPlayer++)
		{
			curMatcher = curTeam[team].teamMembers[curPlayer];
			teamDeaths += curMatcher.downedMe + curMatcher.killedMe + curMatcher.downedMate + curMatcher.killedMate;
			myDeaths += curMatcher.downedMe + curMatcher.killedMe;
			teamKills += curMatcher.downedByMe + curMatcher.killedByMe + curMatcher.downedByMate + curMatcher.killedByMate;
			myKills += curMatcher.downedByMe + curMatcher.killedByMe;
		}
	}
	
	curMatcher = curTeam[myTeam].teamMembers[myNmb];
	extraction += curMatcher.teamextraction;
	bounty += curMatcher.bountyExtracted;
	output<<curMatcher.name<<","<<curMatcher.mmr<<","<<myKills<<","<<myDeaths<<","<<assists<<",";
	
	for(int curPlayer = 0; curPlayer < 3; curPlayer++)
	{
		if(curPlayer == myNmb)continue;
		if(curPlayer >= curTeam[myTeam].teamSize)
		{
			output<<",,";
		}
		else
		{
			curMatcher = curTeam[myTeam].teamMembers[curPlayer];
			output<<curMatcher.name<<","<<curMatcher.mmr<<",";			
			extraction += curMatcher.teamextraction;
			bounty += curMatcher.bountyExtracted;
		}
	}
	
	output<<extraction<<","<<bounty<<","<<teamKills<<","<<teamDeaths<<",";
	for(int team = 0; team < numTeams; team++)
	{
		if(team == myTeam)continue;
		for(int curPlayer = 0; curPlayer < curTeam[team].teamSize; curPlayer++)
		{
			curMatcher = curTeam[team].teamMembers[curPlayer];
			int killCount = 0;
			while(killCount < (curMatcher.downedByMe + curMatcher.killedByMe))
			{
				output<<"+"<<curMatcher.mmr<<",";
				killCount++;
			}
			killCount = 0;
			while(killCount < (curMatcher.downedMe + curMatcher.killedMe))
			{
				output<<"-"<<curMatcher.mmr<<",";
				killCount++;
			}
		}
	}
	output<<"\n";
	output.close();
	return;
}

bool PrintToFile(team* teamsInvolved, int numTeams, int assists, int dMMR)
{
	std::ofstream output;
	output.open("./ScreenMirror.txt", std::ios::out|std::ios::app|std::ios::ate);
	if(!output.good())
	{
		std::cerr<<"Screen mirror could not be opened\n";
		return false;
	}
	
	output<<std::setw(56)<< char(43) <<std::setfill(char(45))<<std::setw(20)<<char(43);
	output<<std::setw(16)<<char(43)<<"\n";
	output.fill(' ');
	output<<std::setw(56)<<char(124)<<std::setw(19)<<"Kills       "<<char(124)<<std::setw(15)<<"Deaths    "<<char(124)<<"\n";
	output.fill(char(45));
	output<<char(43)<<std::setw(34)<<char(43)<<std::setw(14)<<char(43)<<std::setw(7)<<char(43);
	output<<std::setw(10)<<char(43)<<std::setw(10)<<char(43);
	output<<std::setw(8)<<char(43)<<std::setw(8)<<char(43);
	output<<std::setw(10)<<char(43)<<std::setw(13)<<char(43)<<"\n";
	
	output.fill(' ');
	
	output<<char(124)<<std::setw(32)<<"Name"<<" "<<char(124);
	output<<std::setw(12)<<"ID"<<" "<<char(124);
	output<<std::setw(5)<<"MMR"<<" "<<char(124);
	output<<std::setw(8)<<"by me "<<" "<<char(124);
	output<<std::setw(8)<<"by mate"<<" "<<char(124);
	output<<std::setw(6)<<"me "<<" "<<char(124);
	output<<std::setw(6)<<"mate"<<" "<<char(124);
	output<<std::setw(8)<<"Bounty"<<" "<<char(124);
	output<<std::setw(11)<<"Extracted"<<" "<<char(124)<<"\n";
	for(int teamNmb = 0; teamNmb < numTeams; teamNmb++)
	{
		output.fill(char(45));
		output<<char(43)<<std::setw(34)<<char(43);
		output<<std::setw(14)<<char(43);
		output<<std::setw(7)<<char(43);
		output<<std::setw(10)<<char(43);
		output<<std::setw(10)<<char(43);
		output<<std::setw(8)<<char(43);
		output<<std::setw(8)<<char(43);
		output<<std::setw(10)<<char(43);
		output<<std::setw(13)<<char(43)<<"\n";
		output.fill(' ');
		PrintTeamsToFile(teamsInvolved[teamNmb], output);
	}
	output.fill(char(45));
	output<<char(43)<<std::setw(34)<<char(43);
	output<<std::setw(14)<<char(43);
	output<<std::setw(7)<<char(43);
	output<<std::setw(10)<<char(43);
	output<<std::setw(10)<<char(43);
	output<<std::setw(8)<<char(43);
	output<<std::setw(8)<<char(43);
	output<<std::setw(10)<<char(43);
	output<<std::setw(13)<<char(43)<<"\n\n";
	output.fill(' ');
	
	output<<"Assists: "<<assists<<"\n";
	output<<"Estimated new MMR: "<<dMMR<<"\n\n";
	
	return true;
}

void PrintTeamsToFile(team curTeam, std::ofstream& output)
{
	for(int i = 0; i < curTeam.teamSize; i++)
	{
		PrintPlayersToFile(curTeam.teamMembers[i], output);
	}
	return;
}

void PrintPlayersToFile(player curPlayer, std::ofstream& output)
{
	int killedByMe = curPlayer.downedByMe + curPlayer.killedByMe;
	int killedByMate = curPlayer.downedByMate + curPlayer.killedByMate;
	int killedMe = curPlayer.downedMe + curPlayer.killedMe;
	int killedMate = curPlayer.downedMate + curPlayer.killedMate;
	
	output<<char(124)<<std::setw(32)<<curPlayer.name<<" "<<char(124);
	output<<std::setw(12)<<curPlayer.profileID<<" "<<char(124);
	output<<std::setw(5)<<curPlayer.mmr<<" "<<char(124);
	output<<std::setw(8);
	if(killedByMe != 0)
	{
		output<<killedByMe;
	}else
	{
		output<<" ";
	}
	output<<" "<<char(124)<<std::setw(8);
	if(killedByMate != 0)
	{
		output<<killedByMate;
	}else
	{
		output<<" ";
	}
	output<<" "<<char(124)<<std::setw(6);
	if(killedMe != 0)
	{
		output<<killedMe;
	}else
	{
		output<<" ";
	}
	output<<" "<<char(124)<<std::setw(6);
	if(killedMate != 0)
	{
		output<<killedMate;
	}else
	{
		output<<" ";
	}
	output<<" "<<char(124)<<std::setw(8);
	if(curPlayer.bountyExtracted != 0)
	{
		output<<curPlayer.bountyExtracted;
	}else
	{
		output<<" ";
	}
	output<<" "<<char(124)<<std::setw(11);
	if(curPlayer.teamextraction != 0)
	{
		output<<curPlayer.teamextraction;
	}else
	{
		output<<" ";
	}
	output<<" "<<char(124)<<"\n";
	return;
}

int EstimateMMR(match& curMatch)
{
	int estimateMMR = curMatch.ownMMR;
	for(int team=0; team < curMatch.numTeams; team++)
	{
		if(curMatch.teamsInvolved[team].ownTeam == true)continue;
		for(int curPlayer=0; curPlayer < curMatch.teamsInvolved[team].teamSize; curPlayer++)
		{
			player aktPlayer = curMatch.teamsInvolved[team].teamMembers[curPlayer];
			int killedByMe = aktPlayer.downedByMe + aktPlayer.killedByMe;
			int killedMe = aktPlayer.downedMe + aktPlayer.killedMe;
			estimateMMR += (killedMe * DeltaMMR(curMatch.ownMMR, aktPlayer.mmr, true));
			estimateMMR += (killedByMe * DeltaMMR(curMatch.ownMMR, aktPlayer.mmr, false));
		}
	}
	return estimateMMR;
}