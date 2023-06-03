#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>

typedef struct{
	std::string name;
	int profileID;
	int mmr;
	int downedMe;
	int killedMe;
	int downedMate;
	int killedMate;
	int downedByMe;
	int KilledByMe;
	int downedByMate;
	int killedByMate;
}player;

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
	
	char leseZeile[256];
	std::string namensAttribut;
	char nmb[3];
	while(!file.eof())
	{
		file.getline(leseZeile, 256);
		//MissionBagNumTeams zeigt anzahl der Teams an!!
		for(int team=0; team < 12; team++)
		{
			for(int player=0; player < 3; player++)
			{
				namensAttribut = " <Attr name=\"MissionBagPlayer_";
				sprintf(nmb, "%d", team);
				namensAttribut += nmb;
				namensAttribut += "_";
				sprintf(nmb, "%d", player);
				namensAttribut += nmb;
				namensAttribut += "_blood_line_name\" value=";
				//std::cout<<"Attribut = "<<namensAttribut<<"\n";
				//if(strncmp(leseZeile, namensAttribut, namensAttribut.length()) == 0)
				if(namensAttribut.compare(0, namensAttribut.length(), leseZeile, namensAttribut.length()) == 0)
				{
					std::cout<<"Name ["<<team<<", "<<player<<"] = "<<leseZeile + namensAttribut.length()<<"\n";
				}
			}
		}
	}
	system("PAUSE");
	return 0;
}