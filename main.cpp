#include "Stdafx.h"

struct pos {
	int x;
	int y;
};
enum friendly {
	적대,
	중립,
	우호
};
enum typeNPC {
	모험가,
	상인,
	성직자
};
enum trade {
	Atk,
	Def,
	Dex,
	Int
};
enum gameState {
	모험중,
	인카운터_적대,
	인카운터_중립,
	인카운터_우호,
	미방문_마을, // DLC
	미방문_보스몹,
	미방문_우호,
	방문_마을,
	방문_보스몹,
	방문_우호,
};
enum mapActiveState{
	비활성화
	,활성화
	,활성화1
	,활성화2
	,활성화3
	,리셋
};
enum mapType {
	마을
	,마왕성
	,강
	,산
	,바다
	,들판
	,숲
};


class GUI_Support { // GUI를 출력할때 도움을 주는 클래스입니다.
public:
	static void gotoXY(int x, int y) // x, y 좌표에 입력할 수 있도록 해줍니다.
	{
		COORD pos;
		pos.X = x;
		pos.Y = y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	}

	static void setcolor(int color, int bgColor) // 텍스트 생상과 배경 생상을 정해줍니다.
	{
		color &= 0xf;
		bgColor &= 0xf;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bgColor << 4) | color);
	}

	static void windowline(int set_x, int set_y, int pos_x, int pos_y) { // 네모난 창을 쉽게 만들어주는 툴입니다.
		for (int y = 0; y < set_y; y++) {
			for (int x = 0; x < set_x; x += 2) {
				gotoXY(pos_x + x, pos_y + y);
				if (y == 0 || y == set_y - 1) {
					cout << "■";
				}
				else if (x == 0 || x == set_x - 2) {
					cout << "■";
				}
			}
		}
	}

};
class Player {
private:
	string _name;
	int _level;
	int _atk;
	int _def;
	int _dex;
	int _int;

	int _hp;
	int _mp;
	int _exp;

	int _gold;

	pos position;
	pos worldpos;

	friend class IncounterNPC;

public:
	Player(string name) {
		_name = name;
		_level = 1;
		_atk = 5;
		_def = 5;
		_dex = 5;
		_int = 5;

		_hp = _atk * 10;
		_mp = _int * 5;
		_exp = 0;
		_gold = 0;
		position.x = 0;
		position.y = 0;

		worldpos.x = 0;
		worldpos.y = 0;
	}

	string getName() {
		return _name;
	}

	int getLevel() {
		return _level;
	}

	int getAtk() {
		return _atk;
	}

	int getDef() {
		return _def;
	}

	int getDex() {
		return _dex;
	}

	int getInt() {
		return _int;
	}

	int getHp() {
		return _hp;
	}

	int getMp() {
		return _mp;
	}

	int getExp() {
		return _exp;
	}

	int getGold() {
		return _gold;
	}

	pos getWPos() {
		return worldpos;
	}

	pos moveWPos(int x, int y) {
		worldpos.x += x;
		worldpos.y += y;

		if (worldpos.x > 2) {
			worldpos.x = -2;
		}
		else if (worldpos.x < -2) {
			worldpos.x = 2;
		}
		else if (worldpos.y > 2) {
			worldpos.y = -2;
		}
		else if (worldpos.y < -2) {
			worldpos.y = 2;
		}

		return worldpos;
	}

	pos getPos() {
		return position;
	}

	pos move(int x, int y) {
		position.x += x;
		position.y += y;

		if (position.x > 6) {
			moveWPos(1,0);
			position.x = -6;
		}
		else if (position.x < -6) {
			moveWPos(-1, 0);
			position.x = 6;
		}
		else if (position.y > 6) {
			moveWPos(0,1);
			position.y = -6;
		}
		else if (position.y < -6) {
			moveWPos(0, -1);
			position.y = 6;
		}

		return position;
	}


	int attackPhysics(int damage) {
		return damage * (_atk * 0.05f + _dex * 0.3f) + _atk;
	}

	int attackMagic(int damage) {
		if (_mp < 5) {
			return 0;
		}
		_mp -= 5;
		return (damage* 2) * (_int * 0.05f + _dex * 0.3f) + (_int * 2);
	}

	int defencePhysics(int damage) {
		if ((_def * 0.8f + _atk * 0.2f) >= 100) {
			damage = 0;
		}
		else {
			damage *= (100 - (_def * 0.8f + _atk * 0.2f)) / 100;
		}
		_hp -= damage;
		return damage;
	}

	int defenceMagic(int damage) {
		if ((_def * 0.3f + _int * 0.7f) >= 100) {
			damage = 0;
		}
		else {
			damage *= (100 - (_def * 0.3f + _int * 0.7f)) / 100;
		}
		_hp -= damage;
		return damage;
	}

	int magicHeal(int heal) {
		_mp -= heal;
		while (_mp < 0) {
			_mp++;
			heal--;
		}
		int temp = _int * heal * 0.4f;
		if (_hp + temp >= _atk * 10) {
			temp = _atk * 10 - _hp;
			_hp = _atk * 10;
			return temp;
		}
		else {
			_hp += temp;
			return temp;
		}
	}

	bool takeExp(int exp) {
		_exp += exp;
		if (_exp >= _level * 10 + 15) {
			_exp -= _level * 10 + 15;
			_level++;
			_atk++;
			_def++;
			_dex++;
			_int++;
			_hp += 10;
			_mp += 5;
			return true;
		}
		return false;
	}

	void takeGold(int gold) {
		_gold += gold;
	}

	void manaRegen() {
		if(_mp < _int * 5)
			_mp += 1;
	}
};
class Incounter {
private:
	int _friendly;
	int _hp;
	int _atk;
	int _def;

public:
	Incounter(int playerLevel) {
		_friendly = rand() % 3 - 1;
		_atk = (playerLevel + 3 - rand() % (playerLevel + 3)) * 3;
		_def = (playerLevel + 3 - rand() % (playerLevel + 3)) * 2;
		_hp = _atk * 10;
	}

	int getFriendly() {
		return _friendly;
	}
	int getHp() {
		return _hp;
	}
	int getAtk() {
		return _atk;
	}
	int getDef() {
		return _def;
	}

	int attackPhysics(int damage) {
		return damage * (_atk * 0.1f) + (_atk * 1);
	}

	bool defencePhysics(int damage) {
		if ((_def * 0.8f + _atk * 0.2f) >= 100) {
			damage = 0;
		}
		else {
			damage *= 1 - (_def * 0.8f + _atk * 0.2f);
		}

		_hp -= damage;

		if (_hp <= 0) {
			return true;
		}

		return false;
	}

	bool defenceMagic(int damage) {
		if ((_def * 0.5f) >= 100) {
			damage = 0;
		}
		else {
			damage *= 1 - (_def * 0.5f);
		}

		_hp -= damage;

		if (_hp <= 0) {
			return true;
		}

		return false;
	}
};
class IncounterMonster : public Incounter {
private:
	friendly _friendly;
	int _level;
	int _hp;
	int _atk;
	int _def;
public:
	IncounterMonster(int playerLevel) : Incounter(playerLevel) {
		_friendly = 적대;
		_level = (playerLevel + 3 - rand() % (playerLevel + 3));
		_hp = _level * 10;
		_atk = _level * 2;
		_def = _level * 1;

	}
	
	void resetMonster(int playerLevel) {
		_friendly = 적대;
		_level = (playerLevel + 3 - rand() % (playerLevel + 3));
		_hp = _level * 10;
		_atk = _level * 2;
		_def = _level * 1;
	}

	int getFriendly() {
		return _friendly;
	}
	int getLevel() {
		return _level;
	}
	int getHp() {
		return _hp;
	}
	int getAtk() {
		return _atk;
	}
	int getDef() {
		return _def;
	}

	void showInfo() {

	}

	int attackPhysics(int damage) {
		return damage * (_atk * 0.1f) + (_atk * 1);
	}
	int defencePhysics(int damage) {
		if ((_def * 0.8f + _atk * 0.2f) >= 100) {
			damage = 0;
		}
		else {
			damage *= (100 - (_def * 0.8f + _atk * 0.2f)) / 100;
		}
		_hp -= damage;
		return damage;
	}
	bool defenceMagic(int damage) {
		if ((_def * 0.5f) >= 100) {
			damage = 0;
		}
		else {
			damage *= (100 - (_def * 0.5f)) / 100;
		}

		_hp -= damage;

		if (_hp <= 0) {
			return true;
		}

		return false;
	}
};
class IncounterNeutrality : public Incounter {
private:
	friendly _friendly;
	int _hp;
	int _atk;
	int _def;
public:
	IncounterNeutrality(int playerLevel) : Incounter(playerLevel) {
		_friendly = 중립;
		_atk = (playerLevel + 3 - rand() % (playerLevel + 3)) * 1;
		_def = (playerLevel + 3 - rand() % (playerLevel + 3)) * 0.5;
		_hp = _atk * 5;
	}

	void resetNeutrality(int playerLevel){
		_friendly = 중립;
		_atk = (playerLevel + 3 - rand() % (playerLevel + 3)) * 1;
		_def = (playerLevel + 3 - rand() % (playerLevel + 3)) * 0.5;
		_hp = _atk * 5;
	}

	int getFriendly() {
		return _friendly;
	}
	int getHp() {
		return _hp;
	}
	int getAtk() {
		return _atk;
	}
	int getDef() {
		return _def;
	}


	int attackPhysics(int damage) {
		return damage * (_atk * 0.1f) + (_atk * 1);
	}
	int defencePhysics(int damage) {
		if ((_def * 0.8f + _atk * 0.2f) >= 100) {
			damage = 0;
		}
		else {
			damage *= (100 - (_def * 0.8f + _atk * 0.2f)) / 100;
		}

		_hp -= damage;

		return damage;
	}
	int defenceMagic(int damage) {
		if ((_def * 0.5f) >= 100) {
			damage = 0;
		}
		else {
			damage *= (100 - (_def * 0.5f)) / 100;
		}

		_hp -= damage;

		return damage;
	}
};
class IncounterNPC : public Incounter {
private:
	friendly _friendly;
	typeNPC _type;
	int _hp;
	int _atk;
	int _def;

public:
	IncounterNPC(int playerLevel) : Incounter(playerLevel) {
		_friendly = 우호;
		_type = typeNPC(rand() % 3);
		_atk = (playerLevel + 3 - rand() % (playerLevel + 3)) * 3;
		_def = (playerLevel + 3 - rand() % (playerLevel + 3)) * 2;
		_hp = _atk * 10;
	}

	void resetNPC(int playerLevel) {
		_friendly = 우호;
		_type = typeNPC(rand() % 3);
		_atk = (playerLevel + 3 - rand() % (playerLevel + 3)) * 3;
		_def = (playerLevel + 3 - rand() % (playerLevel + 3)) * 2;
		_hp = _atk * 10;
	}

	int getFriendly() {
		return _friendly;
	}
	int getTypeNPC() {
		return _type;
	}
	int getHp() {
		return _hp;
	}
	int getAtk() {
		return _atk;
	}
	int getDef() {
		return _def;
	}


	int attackPhysics(int damage) {
		return damage * (_atk * 0.1f) + (_atk * 1);
	}
	int defencePhysics(int damage) {
		if ((_def * 0.8f + _atk * 0.2f) >= 100) {
			damage = 0;
		}
		else {
			damage *= (100 - (_def * 0.8f + _atk * 0.2f)) / 100;
		}

		_hp -= damage;

		return damage;
	}
	int defenceMagic(int damage) {
		if ((_def * 0.5f) >= 100) {
			damage = 0;
		}
		else {
			damage *= (100 - (_def * 0.5f)) / 100;
		}

		_hp -= damage;

		return damage;
	}
	void tradeList(Player& player) {
		GUI_Support::gotoXY(34, 11);
		cout << "[w]Atk ++ : " << 3 + player.getLevel() * 5 << "Gold" << endl;
		GUI_Support::gotoXY(34, 12);
		cout << "[s]Def ++ : " << 3 + player.getLevel() * 3 << "Gold" << endl;
		GUI_Support::gotoXY(34, 13);
		cout << "[a]Dex ++ : " << 3 + player.getLevel() * 5 << "Gold" << endl;
		GUI_Support::gotoXY(34, 14);
		cout << "[d]Int ++ : " << 3 + player.getLevel() * 3 << "Gold" << endl;
	}
	bool tradeState(Player& player, trade select) {
		switch (select) {
		case Atk:
			if (player.getGold() >= 3 + player.getLevel() * 5) {
				player._gold -= 3 + player.getLevel() * 5;
				player._atk++;
				player._hp += 10;
				return true;
			}
			break;
		case Def:
			if (player.getGold() >= 3 + player.getLevel() * 3) {
				player._gold -= 3 + player.getLevel() * 3;
				player._def++;
				return true;
			}
			break;
		case Dex:
			if (player.getGold() >= 3 + player._level * 5) {
				player._gold -= 3 + player._level * 5;
				player._dex++;
				return true;
			}
			break;
		case Int:
			if (player.getGold() >= 3 + player._level * 3) {
				player._gold -= 3 + player._level * 3;
				player._int++;
				return true;
			}
			break;
		}
		return false;
	}
	int healing(Player& player) {
		if (player._atk * 10 >= player._atk * 3 + player._hp) {
			player._hp += player._atk * 3;
			return player._atk * 3;
		}
		else {
			int temp = player._atk * 10 - player._hp;
			player._hp = player._atk * 10;
			return temp;
		}
	}
};
class Map {
private:
	int map2D[15][15];
	int difficulty;
public:
	Map() {
		difficulty = 0;
		for (int x = 0; x < 13; x++) {
			for (int y = 0; y < 13; y++) {
				map2D[x][y] = rand() % 10;
			}
		}
	}

	Map(int playerLevl) {
		difficulty = playerLevl;
		for (int x = 0; x < 13; x++) {
			for (int y = 0; y < 13; y++) {
				map2D[x][y] = rand() % 10;
			}
		}
	}

	void setMap(int playerLevl) {
		difficulty = playerLevl;
		for (int x = 0; x < 13; x++) {
			for (int y = 0; y < 13; y++) {
				map2D[x][y] = rand() % 10;
			}
		}
	}

	int miniMap(pos playerPosition) {
		GUI_Support::windowline(30, 15, 0, 10);
		int forReturn = 0;
		for (int x = 0; x < 13; x++) {
			for (int y = 0; y < 13; y++) {
				GUI_Support::gotoXY(2 * x + 2, y + 11);
				GUI_Support::setcolor(14, 0);
				if (playerPosition.x + 6 == x && playerPosition.y + 6 == y) {
					GUI_Support::setcolor(10, 0);
					cout << "●";
					forReturn = map2D[x][y];
					map2D[x][y] = 10;
					GUI_Support::setcolor(14, 0);
				}
				else if (map2D[x][y] == 10) {
					cout << "□";
				}
				else {
					cout << "■";
				}
			}
		}
		GUI_Support::gotoXY(10, 10);
		cout << " Area Map ";
		GUI_Support::setcolor(15, 0);
		return forReturn;
	}

};

class WorldMap {
private:
	Map worldmap2D[5][5];
	mapActiveState worldmapData[5][5];

public:
	
	WorldMap(int level) {

		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				worldmap2D[x][y].setMap(1);
				worldmapData[x][y] = mapActiveState(비활성화);
			}
		}
	}

	int ShowWorldMap(pos playerWorldPosition, int &count) {
		const int howPlay = 200;
		count++;
		GUI_Support::gotoXY(2, 25);
		int forReturn = 0;
		for (int x = 0; x < 5; x++) {
			for (int y = 0; y < 5; y++) {
				GUI_Support::gotoXY(2 * x + 2, y + 25);
				GUI_Support::setcolor(15, 0);
				if (playerWorldPosition.x + 2 == x && playerWorldPosition.y + 2 == y) {
					GUI_Support::setcolor(9, 0);
					cout << "◎";
					forReturn = worldmapData[x][y];
					worldmapData[x][y] = mapActiveState(활성화);
					GUI_Support::setcolor(14, 0);
				}
				else if (worldmapData[x][y] == mapActiveState(활성화)) {
					if (count >= howPlay) {
						worldmapData[x][y] = mapActiveState(worldmapData[x][y] + 1);
					}
					cout << "□";
				}
				else if (worldmapData[x][y] == mapActiveState(활성화1)) {
					if (count >= howPlay) {
						worldmapData[x][y] = mapActiveState(worldmapData[x][y] + 1);
					}
					GUI_Support::setcolor(8, 0);
					cout << "▨";
				}
				else if (worldmapData[x][y] == mapActiveState(활성화2)) {
					if (count >= howPlay) {
						worldmapData[x][y] = mapActiveState(worldmapData[x][y] + 1);
					}
					GUI_Support::setcolor(8, 0);
					cout << "■";
				}
				else if (worldmapData[x][y] == mapActiveState(활성화3)) {
					if (count >= howPlay) {
						worldmapData[x][y] = mapActiveState(worldmapData[x][y] + 1);
					}
					GUI_Support::setcolor(7, 0);
					cout << "■";
				}
				else if (worldmapData[x][y] == mapActiveState(리셋)) {
					if (count >= howPlay) {
						worldmapData[x][y] = mapActiveState(비활성화);
					}
					GUI_Support::setcolor(15, 0);
					cout << "■";
				}
				else {
					cout << "■";
				}
			}
			
		}
		if (count >= howPlay)
			count = 0;
		GUI_Support::gotoXY(10, 24);
		cout << " WorldMap ";
		GUI_Support::setcolor(15, 0);
		return forReturn;
	}

	Map &getAreaMap(pos playerWPos) {
		return worldmap2D[playerWPos.x][playerWPos.y];
	}
};

// 입력 도우미
char charinput() {
	char input;
	GUI_Support::gotoXY(2, 38);
	cin >> input;
	return input;
}
// 게임 기본 창 출력
void gameWindow() {
	GUI_Support::windowline(60, 10, 0, 0);
	GUI_Support::windowline(60, 25, 0, 10);
	GUI_Support::windowline(60, 6, 0, 34);
}

// 체력바 print
void healthbar(int fullHP, int nowHP) {
	float percent10 = (float)fullHP / 10;
	int EA = (int)(nowHP / percent10);

	for (int i = 0; i < 10; i++) {
		switch (EA) {
		case 0:
		case 1:
		case 2:
		case 3:
			GUI_Support::setcolor(12, 0);
			break;
		case 4:
		case 5:
		case 6:
			GUI_Support::setcolor(6, 0);
			break;
		case 7:
		case 8:
		case 9:
		case 10:
			GUI_Support::setcolor(10, 0);
			break;
		}
		if (i < EA) {
			cout << "■";
		}
		else {
			cout << "□";
		}
	}
	GUI_Support::setcolor(15, 0);
}

// class 상태창 print
void playerState(Player player) {
	for (int i = 1; i <= 7; i++) { // 화면 초기화
		GUI_Support::gotoXY(4, i);
		cout << "                                        ";
	}
	GUI_Support::gotoXY(27, 1);
	cout << "Player";
	GUI_Support::gotoXY(4, 2);
	cout << "Name : " << player.getName();
	GUI_Support::gotoXY(4, 3);
	cout << "LV : " << player.getLevel() << "\tExp : " << player.getExp() << " / " << 15 + player.getLevel() * 10;
	GUI_Support::gotoXY(4, 4);
	cout << "HP : " << player.getHp() << " / " << player.getAtk() * 10 << "\tMP : " << player.getMp() << " / " << player.getInt() * 5;
	GUI_Support::gotoXY(4, 5);
	healthbar(player.getAtk() * 10, player.getHp());
	GUI_Support::gotoXY(4, 6);
	cout << "Atk : " << player.getAtk() << " | Def : " << player.getDef() << " | Dex : " << player.getDex() << " | Int : " << player.getInt();
	GUI_Support::gotoXY(4, 7);
	cout << "Gold : " << player.getGold();
}
void MonsterState(IncounterMonster incounter) {
	for (int i = 17; i <= 20; i++) { // 화면 초기화
		GUI_Support::gotoXY(34, i);
		cout << "                    ";
	}
	GUI_Support::gotoXY(34, 17);
	cout << "Monster (" << incounter.getLevel() << ")";
	GUI_Support::gotoXY(34, 18);
	cout << "HP : " << incounter.getHp() << " / " << incounter.getLevel() * 10;
	GUI_Support::gotoXY(34, 19);
	healthbar(incounter.getLevel() * 10, incounter.getHp());
	GUI_Support::gotoXY(34, 20);
	cout << "Atk : " << incounter.getAtk() << " | Def : " << incounter.getDef();
}
void NeutralityState(IncounterNeutrality incounter) {
	for (int i = 17; i <= 20; i++) { // 화면 초기화
		GUI_Support::gotoXY(34, i);
		cout << "                    ";
	}
	GUI_Support::gotoXY(34, 17);
	cout << "Neutrality";
	GUI_Support::gotoXY(34, 18);
	cout << "HP : " << incounter.getHp() << " / " << incounter.getAtk() * 5;
	GUI_Support::gotoXY(34, 19);
	healthbar(incounter.getAtk() * 5, incounter.getHp());
	GUI_Support::gotoXY(34, 20);
	cout << "Atk : " << incounter.getAtk() << " | Def : " << incounter.getDef();
}
void NPCState(IncounterNPC incounter) {
	for (int i = 17; i <= 20; i++) { // 화면 초기화
		GUI_Support::gotoXY(34, i);
		cout << "                    ";
	}
	GUI_Support::gotoXY(34, 17);
	cout << "NPC (";
	switch (incounter.getTypeNPC()) {
	case 모험가:
		cout << "모험가)";
		break;
	case 상인:
		cout << "상인)";
		break;
	case 성직자:
		cout << "성직자)";
		break;
	}
	GUI_Support::gotoXY(34, 18);
	cout << "HP : " << incounter.getHp() << " / " << incounter.getAtk() * 10;
	GUI_Support::gotoXY(34, 19);
	healthbar(incounter.getAtk() * 10, incounter.getHp());
	GUI_Support::gotoXY(34, 20);
	cout << "Atk : " << incounter.getAtk() << " | Def : " << incounter.getDef();
}

// 컨트롤, 로그 print
void controllkey(gameState state) {
	for (int i = 11; i <= 15; i++) {
		GUI_Support::gotoXY(34, i);
		cout << "                        ";
	}

	switch (state) {
	case 모험중:
		GUI_Support::setcolor(15, 0);
		GUI_Support::gotoXY(38, 10);
		cout << " 모험컨트롤 ";
		GUI_Support::gotoXY(41, 11);
		cout << "상 : w";
		GUI_Support::gotoXY(41, 15);
		cout << "하 : s";
		GUI_Support::gotoXY(34, 13);
		cout << "좌 : a";
		GUI_Support::gotoXY(47, 13);
		cout << "우 : d";
		GUI_Support::gotoXY(2, 38);
		break;
	case 인카운터_적대:
		GUI_Support::setcolor(12, 0);
		GUI_Support::gotoXY(38, 10);
		cout << " 전투컨트롤 ";
		GUI_Support::gotoXY(34, 11);
		cout << "공격하기 : w";
		GUI_Support::gotoXY(34, 12);
		cout << "방어하기 : s";
		GUI_Support::gotoXY(34, 13);
		cout << "마법공격 : a";
		GUI_Support::gotoXY(34, 14);
		cout << "마법치료 : d";
		GUI_Support::gotoXY(2, 38);
		break;
	case 인카운터_중립:
		GUI_Support::setcolor(14, 0);
		GUI_Support::gotoXY(38, 10);
		cout << " 동물컨트롤 ";
		GUI_Support::gotoXY(34, 11);
		cout << "공격하기 : w";
		GUI_Support::gotoXY(34, 14);
		cout << "무시하기 : d";
		GUI_Support::gotoXY(2, 38);
		break;
	case 인카운터_우호:
		GUI_Support::gotoXY(38, 10);
		cout << " 대화컨트롤 ";
		GUI_Support::setcolor(12, 0);
		GUI_Support::gotoXY(34, 11);
		cout << "공격하기 : w";
		GUI_Support::setcolor(11, 0);
		GUI_Support::gotoXY(34, 12);
		cout << "대화하기 : s";
		GUI_Support::setcolor(14, 0);
		GUI_Support::gotoXY(34, 14);
		cout << "무시하기 : d";
		GUI_Support::gotoXY(2, 38);
		break;
	}
	GUI_Support::setcolor(15, 0);
}
void printLog(string log[3]) {
	Sleep(200);
	for (int i = 35; i <= 38; i++) {
		GUI_Support::gotoXY(2, i);
		cout << "                                                    ";
	}
	GUI_Support::setcolor(8, 0);
	GUI_Support::gotoXY(2, 35);
	cout << log[2]; // 로그 출력

	GUI_Support::setcolor(7, 0);
	GUI_Support::gotoXY(2, 36);
	cout << log[1]; // 로그 출력

	GUI_Support::setcolor(15, 0);
	GUI_Support::gotoXY(2, 37);
	cout << log[0]; // 로그 출력

	log[2] = log[1]; // 1차 로그 밀어내기
	log[1] = log[0];
	GUI_Support::setcolor(15, 0); // 색 초기화
}

// 각 class간의 전투 프로세스
bool battleMonster(Player &player, IncounterMonster &monster, string log[3]) {
	char input = ' ';
	int dice = 0;
	while (player.getHp() > 0 && monster.getHp() > 0) {
		MonsterState(monster);
		playerState(player);
		input = charinput();
		dice = rand() % 6 + 1;
		switch(input) {
		case 'w':
			log[0] = "[" + to_string(dice) + "]몬스터에게 " + to_string(monster.defencePhysics(player.attackPhysics(dice))) + "의 대미지를 주었습니다.";
			break;
		case 's':
			dice = rand() % 6 + 1;
			log[0] = "[" + to_string(dice) + "]몬스터의 공격을 막아 " + to_string(player.defencePhysics(monster.attackPhysics(dice / 2))) + "의 대미지를 받았습니다.";
			break;
		case 'a':
			log[0] = "[" + to_string(dice) + "]몬스터에게 " + to_string(monster.defencePhysics(player.attackMagic(dice))) + "의 마법공격을 했습니다.";
			break;
		case 'd':
			log[0] = "[" + to_string(dice) + "]" + to_string(player.magicHeal(dice)) + "만큼 회복했습니다.";
			break;
		}
		printLog(log);
		Sleep(500);
		if (input != 's') {
			dice = rand() % 6 + 1;
			log[0] = "[" + to_string(dice) + "]몬스터의 공격을 받아" + to_string(player.defencePhysics(monster.attackPhysics(dice))) + "의 대미지를 받았습니다.";
		}
		else {
			log[0] = "";
		}
		printLog(log);
	}
	for (int i = 17; i <= 20; i++) { // 화면 초기화
		GUI_Support::gotoXY(34, i);
		cout << "                    ";
	}
	log[0] = "전투가 종료되었습니다.";
	return true;
}
bool battleNeutrality(Player& player, IncounterNeutrality& neutrality, string log[3]) {
	controllkey(인카운터_적대);
	char input = ' ';
	int dice = 0;
	while (player.getHp() > 0 && neutrality.getHp() > 0) {
		NeutralityState(neutrality);
		playerState(player);
		input = charinput();
		dice = rand() % 6 + 1;
		switch (input) {
		case 'w':
			log[0] = "[" + to_string(dice) + "]중립몹에게 " + to_string(neutrality.defencePhysics(player.attackPhysics(dice))) + "의 대미지를 주었습니다.";
			break;
		case 's':
			dice = rand() % 6 + 1;
			log[0] = "[" + to_string(dice) + "]중립몹의 공격을 막아 " + to_string(player.defencePhysics(neutrality.attackPhysics(dice / 2))) + "의 대미지를 받았습니다.";
			break;
		case 'a':
			log[0] = "[" + to_string(dice) + "]중립몹에게 " + to_string(neutrality.defencePhysics(player.attackMagic(dice))) + "의 마법공격을 했습니다.";
			break;
		case 'd':
			log[0] = "[" + to_string(dice) + "]" + to_string(player.magicHeal(dice)) + "만큼 회복했습니다.";
			break;
		}
		printLog(log);
		Sleep(1000);
		if (input != 's') {
			dice = rand() % 6 + 1;
			log[0] = "[" + to_string(dice) + "]중립몹의 공격을 받아" + to_string(player.defencePhysics(neutrality.attackPhysics(dice))) + "의 대미지를 받았습니다.";
		}
		else {
			log[0] = "";
		}
		printLog(log);
	
	}
	for (int i = 17; i <= 20; i++) { // 화면 초기화
		GUI_Support::gotoXY(34, i);
		cout << "                    ";
	}
	log[0] = "전투가 종료되었습니다.";
	return true;
}
bool battleNPC(Player& player, IncounterNPC& npc, string log[3]) {
	controllkey(인카운터_적대);
	char input = ' ';
	int dice = 0;
	while (player.getHp() > 0 && npc.getHp() > 0) {
		NPCState(npc);
		playerState(player);
		input = charinput();
		dice = rand() % 6 + 1;
		switch (input) {
		case 'w':
			log[0] = "[" + to_string(dice) + "]NPC에게 " + to_string(npc.defencePhysics(player.attackPhysics(dice))) + "의 대미지를 주었습니다.";
			break;
		case 's':
			dice = rand() % 6 + 1;
			log[0] = "[" + to_string(dice) + "]NPC의 공격을 막아 " + to_string(player.defencePhysics(npc.attackPhysics(dice / 2))) + "의 대미지를 받았습니다.";
			break;
		case 'a':
			log[0] = "[" + to_string(dice) + "]NPC에게 " + to_string(npc.defencePhysics(player.attackMagic(dice))) + "의 마법공격을 했습니다.";
			break;
		case 'd':
			log[0] = "[" + to_string(dice) + "]" + to_string(player.magicHeal(dice)) + "만큼 회복했습니다.";
			break;
		}
		printLog(log);
		Sleep(1000);
		if (input != 's') {
			dice = rand() % 6 + 1;
			log[0] = "[" + to_string(dice) + "]NPC의 공격을 받아" + to_string(player.defencePhysics(npc.attackPhysics(dice))) + "의 대미지를 받았습니다.";
		}
		else {
			log[0] = "";
		}
		printLog(log);
	}
	for (int i = 17; i <= 20; i++) { // 화면 초기화
		GUI_Support::gotoXY(34, i);
		cout << "                    ";
	}
	log[0] = "전투가 종료되었습니다.";
	return true;
}

// player 이동 프로세스
void movePlayer(Player &player, gameState &state, WorldMap &map, string log[3]) {
	char input = ' ';
	int forState = 0;
	input = charinput();
	switch (input) {
	case 'w':
		forState = map.getAreaMap(player.getWPos()).miniMap(player.move(0, -1));
		log[0] = "플레이어가 위로 이동하였습니다.";
		break;
	case 'a':
		forState = map.getAreaMap(player.getWPos()).miniMap(player.move(-1, 0));
		log[0] = "플레이어가 좌로 이동하였습니다.";
		break;
	case 's':
		forState = map.getAreaMap(player.getWPos()).miniMap(player.move(0, 1));
		log[0] = "플레이어가 아래로 이동하였습니다.";
		break;
	case 'd':
		forState = map.getAreaMap(player.getWPos()).miniMap(player.move(1, 0));
		log[0] = "플레이어가 우로 이동하였습니다.";
		break;
	}
	if (forState >= 4) {
		state = gameState(0);
	}
	else {
		state = gameState(forState);
	}
}



int main() {
	// 시드 초기화
	srand(time(NULL));
	
	// 콘솔창 크기 조절 및 색 조정
	system("mode con: cols=80 lines=40");
	GUI_Support::setcolor(15, 0);

	// 요구되는 변수
	char input = ' ';
	string lastestActivity[3] = { "","" }; // 활동 로그 저장 변수
	gameState state = 모험중;
	int forState = 0;
	trade forTrade;
	bool isDead = false;
	int count = 0;

	// class 선언 전 플레이어 이름 입력
	string inputName;
	GUI_Support::windowline(60, 5, 0, 18);
	GUI_Support::gotoXY(2, 20);
	cout << "플레이어 이름을 입력하세요 : ";
	cin >> inputName;

	// class 선언부
	Player player(inputName);
	IncounterMonster monster(player.getLevel());
	IncounterNeutrality neutrality(player.getLevel());
	IncounterNPC npc(player.getLevel());
	//Map map(player.getLevel());
	WorldMap worldMap(1);
	// 게임화면 진입 전 화면정리
	system("cls");

	// 게임화면 진입
	while (input != 'q') { // q 입력시 게임 종료
		worldMap.ShowWorldMap(player.getWPos(), count);
		gameWindow(); // 게임화면 기본 창 출력
		GUI_Support::windowline(32, 9, 28, 16); // 대상 정보창 출력
		playerState(player); // 플레이어 현상황 출력
		//map.miniMap(player.getPos()); // 미니맵 출력
		worldMap.getAreaMap(player.getWPos()).miniMap(player.getPos());

		controllkey(state); // 현 상황에 대한 컨트롤 설명표 출력
		
		player.manaRegen();
		
		switch (state) { // 현 상황에 대한 분기
		case 모험중: // 플레이어 이동만 가능
			movePlayer(player, state, worldMap, lastestActivity);
			break;

		case 인카운터_적대: // 전투만 가능
			MonsterState(monster);
			lastestActivity[0] = "플레이어가 몬스터를 만났습니다.";
			if (isDead = battleMonster(player, monster, lastestActivity)) {
				state = gameState(모험중);
				isDead = false;
				monster.resetMonster(player.getLevel());
				player.takeExp(monster.getLevel() * 3);
				player.takeGold(monster.getLevel() * 2);
			}
			break;

		case 인카운터_중립: // 전투 또는 회피 가능
			NeutralityState(neutrality);
			lastestActivity[0] = "플레이어가 중립몹을 만났습니다.";
			input = charinput();
			switch (input) {
			case 'w':
				lastestActivity[0] = "플레이어가 중립몹과 전투를 시작했습니다.";
				if (isDead = battleNeutrality(player, neutrality, lastestActivity)) {
					state = gameState(모험중);
					isDead = false;
					neutrality.resetNeutrality(player.getLevel());
					player.takeExp(neutrality.getAtk());
					player.takeGold(neutrality.getDef());
				}
				break;
			case 's':

				break;
			case 'd':
				state = 모험중;
				neutrality.resetNeutrality(player.getLevel());
				break;
			}

			break;

		case 인카운터_우호: // 전투, 거래, 치료, 회피 가능
			NPCState(npc);
			lastestActivity[0] = "플레이어가 NPC를 만났습니다.";
			printLog(lastestActivity);
			input = charinput();
			switch(input) {
			case 'w':
				lastestActivity[0] = "플레이어가 NPC와 전투를 시작했습니다.";
				if (isDead = battleNPC(player, npc, lastestActivity)) {
					state = gameState(모험중);
					isDead = false;
					npc.resetNPC(player.getLevel());
					player.takeExp(npc.getAtk() * 2);
					player.takeGold(npc.getDef());
				}
				break;
			case 's':
				switch (npc.getTypeNPC()) {
				case 모험가:
					break;
				case 상인:
					npc.tradeList(player);
					input = charinput();
					switch (input) {
					case 'w':
						forTrade = Atk;
						break;
					case 's':
						forTrade = Def;
						break;
					case 'a':
						forTrade = Dex;
						break;
					case 'd':
						forTrade = Int;
						break;
					}
					if (npc.tradeState(player, forTrade)) {
						lastestActivity[0] = "NPC와의 거래에 성공하였습니다.";
					}
					else {
						lastestActivity[0] = "NPC와의 거래에 실패하였습니다.";
					}
					break;
				case 성직자:
					if (player.getGold() >= 10) {
						player.takeGold(rand() % (int)(player.getGold() / 10));
						lastestActivity[0] = "플레이어가 성직자에게 모금하였습니다.";
						printLog(lastestActivity);
					}
					lastestActivity[0] = "성직자가 플레이어를 치료해주었습니다.";
					npc.healing(player);
					state = 모험중;
					break;
				}
				state = 모험중;
				npc.resetNPC(player.getLevel());
				break;
			case 'd':
				state = 모험중;
				npc.resetNPC(player.getLevel());
				break;
			}
		}
		for (int t = 17; t <= 20; t++) { // 화면 초기화
			GUI_Support::gotoXY(34, t);
			cout << "                    ";
		}
		if (player.getHp() <= 0) {
			lastestActivity[0] = "플레이어가 사망하였습니다.";
			printLog(lastestActivity);
			break;
		}

		//system("cls"); // 화면 정리
		printLog(lastestActivity);

	}
	cout << "\n게임을 종료하시려면 아무거나 입력하세요.";
	cin >> input;

	return 0;
}
