#include <bits/stdc++.h>
#define pi pair<int, int>
#define x first
#define y second

using namespace std;

int numberMineSpots, gold, income, opponentGold, opponentIncome, myMineCount;
char grid[12][12];
int mines[12][12], myMines[12][12], dist[12][12], enemydist[12][12], buildings[12][12], enemyTowerCover[12][12];
int trainGold[4] = {0, 10, 20, 30}, trainIncome[4] = {0, 1, 4, 20};
pi myHQ, enemyHQ;
pi units[12][12];
vector<pi> minepos, unitpos, trainpos, adj[12][12];

inline bool ingrid(int i, int j) {
	return (i >= 0 && i < 12 && j >= 0 && j < 12 && grid[i][j] != '#');
}

void initadj() {
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			if (!ingrid(i, j))
				continue;
			if (ingrid(i - 1, j))
				adj[i][j].push_back(pi(i - 1, j));
			if (ingrid(i + 1, j))
				adj[i][j].push_back(pi(i + 1, j));
			if (ingrid(i, j - 1))
				adj[i][j].push_back(pi(i, j - 1));
			if (ingrid(i, j + 1))
				adj[i][j].push_back(pi(i, j + 1));
		}
	}
}

void bfs() {
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			dist[i][j] = 1e9;
		}
	}
	dist[myHQ.x][myHQ.y] = 0;
	queue<pi> q;
	q.push(myHQ);
	while (!q.empty()) {
		pi v = q.front();
		q.pop();
		for (pi u : adj[v.x][v.y]) {
			if (dist[u.x][u.y] == 1e9) {
				dist[u.x][u.y] = dist[v.x][v.y] + 1;
				q.push(u);
			}
		}
	}
}

void enemybfs() {
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			enemydist[i][j] = 1e9;
		}
	}
	enemydist[enemyHQ.x][enemyHQ.y] = 0;
	queue<pi> q;
	q.push(enemyHQ);
	while (!q.empty()) {
		pi v = q.front();
		q.pop();
		for (pi u : adj[v.x][v.y]) {
			if (enemydist[u.x][u.y] == 1e9) {
				enemydist[u.x][u.y] = enemydist[v.x][v.y] + 1;
				q.push(u);
			}
		}
	}
}

void computeEnemyTowerCover() {
	memset(enemyTowerCover, 0, sizeof enemyTowerCover);
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			if (buildings[i][j] == -3) {
				enemyTowerCover[i][j] = 1;
				for (pi p : adj[i][j]) {
					if (grid[p.x][p.y] == 'O')
						enemyTowerCover[p.x][p.y] = 1;
				}
			}
		}
	}
}

bool cantrain(int i, int j) {
	if (!ingrid(i, j))
		return false;
	if (units[i][j].first > 0)
		return false;
	if (buildings[i][j] > 0)
		return false;
	if (grid[i][j] == 'O')
		return true;
	for (pi p : adj[i][j]) {
		if (grid[p.x][p.y] == 'O')
			return true;
	}
	return false;
}

void train(int level, int i, int j) {
	grid[i][j] = 'X';
	if (buildings[i][j] == -4) {
		buildings[i][j] = 0;
		computeEnemyTowerCover();
	}
	buildings[i][j] = 0;
	units[i][j] = pi(level, -1);
	gold -= trainGold[level];
	income -= trainIncome[level];
	cout << "TRAIN " << level << " " << i << " " << j << "; ";
}

bool lowdist(pi a, pi b) {
	return dist[a.x][a.y] < dist[b.x][b.y];
}

bool highdist(pi a, pi b) {
	return dist[a.x][a.y] > dist[b.x][b.y];
}

void buildMine() {
	pi pos = minepos[0];
	cout << "BUILD MINE " << pos.x << " " << pos.y << "; ";
	gold -= 20 + 4 * myMineCount;
	myMineCount++;
	myMines[pos.x][pos.y] = 1;
	minepos.erase(minepos.begin());
}

bool canmove(int i, int j, int level) {
	if (units[i][j].first > 0)
		return false;
	if (units[i][j].first < 0) {
		if (level < min(3, -units[i][j].first + 1))
			return false;
	}
	if (buildings[i][j] > 0)
		return false;
	if (enemyTowerCover[i][j] && level < 3)
		return false;
	return true;
}

int priority(int i, int j) {
	if (grid[i][j] == 'X')
		return 4;
	if (grid[i][j] == 'x')
		return 3;
	if (grid[i][j] == '.')
		return 2;
	if (grid[i][j] == 'O')
		return 1;
}

bool movecomp(pi a, pi b) {
	if (buildings[a.x][a.y] != buildings[b.x][b.y])
		return -buildings[a.x][a.y] < -buildings[b.x][b.y];
	int ua = max(-units[a.x][a.y].first, 0);
	int ub = max(-units[b.x][b.y].first, 0);
	if (ua != ub)
		return ua > ub;
	if (priority(a.x, a.y) != priority(b.x, b.y))
		return priority(a.x, a.y) > priority(b.x, b.y);
	if (priority(a.x, a.y) != 1)
		return dist[a.x][a.y] < dist[b.x][b.y];
	else
		return enemydist[a.x][a.y] < enemydist[b.x][b.y];
}

void applymove(int x1, int y1, int x2, int y2) {
	grid[x2][y2] = 'X';
	buildings[x2][y2] = 0;
	units[x2][y2] = units[x1][y1];
	units[x1][y1] = pi(0, 0);
}

int move(int i, int j) {
	int level, id;
	tie(level, id) = units[i][j];
	vector<pi> moves;
	moves.push_back(pi(i, j));
	for (pi p : adj[i][j]) {
		if (canmove(p.x, p.y, level))
			moves.push_back(p);
	}
	sort(moves.begin(), moves.end(), movecomp);
	// cerr << "Moving " << moves[0].first << " " << moves[0].second << ", moves = ";
	// for (pi p : moves) {
	//     cerr << "(" << p.x << ", " << p.y << ") ";
	// cerr << "b = " << buildings[p.x][p.y];
	// cerr << " u = " << units[p.x][p.y].first;
	// cerr << " p = " << priority(p.x, p.y);
	// cerr << " d = " << dist[p.x][p.y];
	// cerr << " ed = " << enemydist[p.x][p.y] << endl;
	// }
	// cerr << endl;
	if (pi(i, j) != moves[0]) {
		applymove(i, j, moves[0].first, moves[0].second);
		cout << "MOVE " << id << " " << moves[0].first << " " << moves[0].second << "; ";
	}
}

bool traincomp(pi a, pi b) {
	if (dist[a.x][a.y] != dist[b.x][b.y])
		return dist[a.x][a.y] < dist[b.x][b.y];
	if (buildings[a.x][a.y] != buildings[b.x][b.y])
		return -buildings[a.x][a.y] < -buildings[b.x][b.y];
	if (units[a.x][a.y].first != units[b.x][b.y].first)
		return -units[a.x][a.y].first > -units[b.x][b.y].first;
	if (priority(a.x, a.y) != priority(b.x, b.y))
		return priority(a.x, a.y) > priority(b.x, b.y);
	return enemydist[a.x][a.y] < enemydist[b.x][b.y];
}

bool gridCovered() {
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			if (grid[i][j] == '.')
				return false;
		}
	}
	return true;
}

void initMinePos() {
	minepos.clear();
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			if (mines[i][j] && !myMines[i][j] && grid[i][j] == 'O')
				minepos.push_back(pi(i, j));
		}
	}
	sort(minepos.begin(), minepos.end(), lowdist);
	cerr << "minepos = ";
	for (pi p : minepos)
		cerr << "(" << p.x << ", " << p.y << ") ";
	cerr << endl;
}

void initTrainPos() {
	trainpos.clear();
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			if (!cantrain(i, j))
				continue;
			if (grid[i][j] != 'O')
				trainpos.push_back(pi(i, j));
		}
	}
	sort(trainpos.begin(), trainpos.end(), traincomp);

	cerr << "trainpos = ";
	for (pi p : trainpos) {
		cerr << "(" << p.x << ", " << p.y << ") ";
	}
	cerr << endl;
}

int main() {
	cin >> numberMineSpots;
	for (int i = 0; i < numberMineSpots; i++) {
		int x, y;
		cin >> x >> y;
		mines[x][y] = 1;
	}

	bool init = false;
	// game loop
	while (1) {
		cin >> gold >> income >> opponentGold >> opponentIncome;
		for (int y = 0; y < 12; y++) {
			string line;
			cin >> line;
			for (int x = 0; x < 12; x++)
				grid[x][y] = line[x];
		}
		int buildingCount;
		cin >> buildingCount;
		myMineCount = 0;
		memset(myMines, 0, sizeof myMines);
		for (int i = 0; i < buildingCount; i++) {
			int owner, type, x, y;
			cin >> owner >> type >> x >> y;
			buildings[x][y] = (type + 1) * (1 - 2 * owner);
			if (type == 0) {
				if (owner == 0)
					myHQ = pi(x, y);
				else
					enemyHQ = pi(x, y);
			}
			else if (type == 1) {
				if (owner == 0) {
					myMines[x][y] = 1;
					myMineCount++;
				}
			}
			else if (type == 2) {
			}
		}
		int unitCount;
		cin >> unitCount;
		for (int i = 0; i < 12; i++)
			for (int j = 0; j < 12; j++)
				units[i][j] = pi(0, 0);
		unitpos.clear();
		for (int i = 0; i < unitCount; i++) {
			int owner, id, level, x, y;
			cin >> owner >> id >> level >> x >> y;
			if (owner == 0) {
				units[x][y] = pi(level, id);
				unitpos.push_back(pi(x, y));
			}
			else {
				units[x][y] = pi(-level, id);
			}
		}

		// input done
		if (!init) {
			initadj();
			bfs();
			enemybfs();
			init = true;
		}

		sort(unitpos.begin(), unitpos.end(), highdist);
		cerr << "unitpos = ";
		for (pi p : unitpos) {
			cerr << "(" << p.x << ", " << p.y << ") ";
		}
		cerr << endl;

		// precomputation done

		for (pi p : unitpos) {
			move(p.x, p.y);
		}
		initMinePos();
		if (gridCovered()) {
			while (minepos.size() > 0 && gold >= 20 + 4 * myMineCount) {
				buildMine();
			}
		}
		initTrainPos();
		for (pi p : trainpos) {
			int level = min(3, -units[p.x][p.y].first + 1);
			if (enemyTowerCover[p.x][p.y])
				level = 3;
			if (gold >= trainGold[level] && income >= trainIncome[level]) {
				train(level, p.x, p.y);
			}
		}

		cout << "WAIT" << endl;
	}
}