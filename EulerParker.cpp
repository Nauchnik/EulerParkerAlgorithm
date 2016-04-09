#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <sstream>

using namespace std;

const int DLS_order = 10;
static inline double cpuTime(void) { return (double)clock() / CLOCKS_PER_SEC; }

/*class DLX_column {
public:
	int size;
	int column_number;
	int row_id;

	DLX_column * Left;
	DLX_column * Right;
	DLX_column * Up;
	DLX_column * Down;
	DLX_column * Column;
};*/

vector<vector<int>> tr(10000, vector<int>(10)); 
vector<int> ltr(10);
vector<int> hb(11);
vector<vector<vector<int>>> orts(10000);
void readDlsFromFile(string dls_file_name, vector<vector<vector<int>>> &dls_vec);
vector<vector<int>> getTrans_mod(vector<vector<int>> a, bool diag);
static bool nextij(int &i, int &j, vector<int> &ct, vector<vector<int>> & a, vector<bool>&co, vector<bool> & x);
bool nextTrans_ext(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x, bool &diag);
//void search(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr);

int main( int argc, char **argv ) {
	bool isSearchingDiagMates = true;

	vector<vector<vector<int>>> dls_vec;
	// DLS from [Brown et al 1992]
	vector<vector<int>> a{ 
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
		{ 2, 3, 4, 9, 8, 1, 0, 5, 6, 7 },
		{ 3, 4, 9, 8, 2, 7, 1, 0, 5, 6 },
		{ 8, 7, 6, 5, 0, 9, 4, 3, 2, 1 },
		{ 5, 0, 1, 7, 6, 3, 2, 8, 9, 4 },
		{ 6, 5, 0, 1, 7, 2, 8, 9, 4, 3 },
		{ 4, 9, 8, 2, 3, 6, 7, 1, 0, 5 },
		{ 7, 6, 5, 0, 1, 8, 9, 4, 3, 2 },
		{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
		{ 1, 2, 3, 4, 9, 0, 5, 6, 7, 8 } 
	};
	dls_vec.push_back(a);

#ifdef _DEBUG
	argc = 2;
	argv[1] = "DLS.txt";
#endif
	if (argc < 2) {
		cerr << "Usage: [file with LSs (or DLSs)] [diagonality condition]" << endl;
		return 1;
	}
	
	if (argc > 1) {
		string dls_file_name = argv[1];
		readDlsFromFile(dls_file_name, dls_vec);
	}
	if (argc > 2) {
		string str = argv[2];
		if ((str == "true") || (str == "1"))
			isSearchingDiagMates = true;
		else if ((str == "false") || (str == "0"))
			isSearchingDiagMates = false;
	}
	cout << "isSearchingDiagMates " << isSearchingDiagMates << endl;
	cout << "dls_vec.size() " << dls_vec.size() << endl;
	double gt1;
	double gt2;
	//double gt3;
	int sum;
	std::vector<std::vector<int>> transv_vec;
	
	for (unsigned dls_vec_index = 0; dls_vec_index < dls_vec.size(); dls_vec_index++) {
		cout << "dls_vec_index " << dls_vec_index << endl;
		gt1 = cpuTime();
		transv_vec = getTrans_mod(dls_vec[dls_vec_index], isSearchingDiagMates);
		gt2 = cpuTime();
		sum = 0;
		for (unsigned i = 0; i < ltr.size(); i++)
			sum += ltr[i];
		cout << "Generation of " << transv_vec.size() << " transversals took " << gt2 - gt1 << " seconds" << endl;
		//int t = getOrt();
		//gt3 = cpuTime();
		//cout << "Generation of orthogonal pairs took " << gt3 - gt2 << " seconds" << endl;
	}
	
	cout << endl << "Finish";
	return 0;
}

void readDlsFromFile(string dls_file_name, vector<vector<vector<int>>> &dls_vec)
{
	string str;
	ifstream dls_file(dls_file_name.c_str());
	if (!dls_file.is_open()) {
		cerr << "can't open " << dls_file_name.c_str() << endl;
		exit(1);
	}

	stringstream sstream;
	int cell_val;
	vector<int> dls_row;
	vector<vector<int>> dls;
	while (getline(dls_file, str)) {
		if (str.size() < 10) continue;
		sstream << str;
		while (sstream >> cell_val) {
			dls_row.push_back(cell_val);
			if (dls_row.size() == DLS_order) {
				dls.push_back(dls_row);
				dls_row.clear();
			}
			if (dls.size() == DLS_order) {
				dls_vec.push_back(dls);
				dls.clear();
			}
		}
		sstream.clear();
		sstream.str("");
	}
	
	dls_file.close();
}

bool initTrans(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x) 
{
	int i = 0;
	int j = -1;
	for (unsigned i = 0; i < x.size(); i++) {
		co[i] = false;
		x[i] = false;
		ct[i] = 0;
	}
	while ((i<10) && (nextij(i, j, ct, a, co, x) != false)) {
		ct[i] = j;
		i++;
		j = -1;
	}
	if (i == 10) return true;
	else return false;
}

vector<vector<int>> getTrans_mod(vector<vector<int>> a, bool diag) 
{
	vector<vector<int>> res(5600, vector<int>(10));
	vector<int> ltr(10);
	vector<int> h(10);
	vector<bool> co(10);
	vector<bool> x(10);

	int num = 0;

	//bool t = initTrans(a, h, co, x);
	bool t = initTrans(a, h, co, x);
	if (t == true) {
		bool acc = true;
		if (diag == true) {
			int md = 0;
			int ad = 0;
			for (unsigned l = 0; l < h.size(); l++) {
				if (h[l] == l) { md++; }
				if (h[l] == h.size() - l - 1) { ad++; }
			}
			if ((md != 1) || (ad != 1)) { acc = false; }
		}
		if (acc == true) {
			res[num] = h;
			num++;
		}

		//	while (nextTrans(a, h, co, x) == true) {
		while (nextTrans_ext(a, h, co, x, diag) == true) {
			res[num] = h;
			num++;
		}
	}
	res.erase(res.begin() + num, res.end());
	return res;
}

bool nextTrans_ext(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x, bool &diag) 
{
	// a - square
	// ct = current transversal, coordinates correspond to column number
	// co - column occupied vector
	// x - aux vector
	int d = ct.size() - 1;
	int i = d - 1;
	x[a[d][ct[d]]] = false;
	x[a[i][ct[i]]] = false;
	co[ct[i]] = false;
	co[ct[d]] = false;
	int j = ct[i];
	bool out = true;
	bool res = false;

	while (!res) {
		if (nextij(i, j, ct, a, co, x) == true) {

			ct[i] = j;
			i++;
			j = -1;
			if ((i == 10) && (diag == false)) { res = true; }
			if ((i == 10) && (diag == true)) {
				int md = 0;
				int ad = 0;
				for (unsigned l = 0; l < ct.size(); l++) {
					if (ct[l] == l) { md++; }
					if (ct[l] == ct.size() - l - 1) { ad++; }
				}
				if ((md == 1) && (ad == 1)) { res = true; }
			}
		}
		else {
			res = true;
			out = false;
		}

	}
	return out;
}

static bool nextij(int &i, int &j, vector<int> &ct, vector<vector<int>> & a, vector<bool>&co, vector<bool> & x) 
{
	int dim = x.size();
	bool r = false;

	while ((r == false) && (j < dim)) {
		j++;
		if (j == 10) {
			i--;
			if (i < 0) {
				r = true;
			}
			else {
				co[ct[i]] = false;
				x[a[i][ct[i]]] = false;
				j = ct[i];
			}
		}
		else {
			if ((co[j] == false) && (x[a[i][j]] == false)) {
				co[j] = true;
				x[a[i][j]] = true;
				r = true;
			}
		}
	}
	if (i < 0) return false;
	else return true;
}

/*
void search(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr) 
{
	//pd = partial solution
	if (k > 10) {
		cout << "we are in trouble" << endl;

	}
	//	cout << "Search " << k << endl;
	if (h.Right == &h) {
		vector<int> tmpv;
		for (int i = 0; i < ps.size(); i++) {
			tmpv.push_back(ps[i]->row_id);
		}
		tvr.push_back(tmpv);
		//cout << tvr.size() << endl;
		//print_solution(ps);
	}
	else {
		DLX_column * c = NULL;
		choose_c(h, c);
		//	cout << "picked column " << c->column_number << endl;
		cover(c);
		DLX_column * r = c->Down;
		while (r != c) {
			ps.push_back(r);
			DLX_column * j;
			j = r->Right;
			while (j != r) {
				cover(j->Column);
				j = j->Right;
			}

			search(k + 1, h, ps, tvr);

			r = ps.back();
			//questionable.
			ps.pop_back();
			c = r->Column;

			j = r->Left;
			while (j != r) {
				uncover(j->Column);
				j = j->Left;
			}
			r = r->Down;
		}
		uncover(c);
		//return;
	}
}
*/