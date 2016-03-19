#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <sstream>

using namespace std;

const int DLS_order = 10;
static inline double cpuTime(void) { return (double)clock() / CLOCKS_PER_SEC; }
vector<vector<int>> tr(10000, vector<int>(10)); 
vector<int> ltr(10);
vector<int> hb(11);
vector<vector<vector<int>>> orts(10000);
void readDlsFromFile(string dls_file_name, vector<vector<vector<int>>> &dls_vec);
int getOrt();
void getTrans(vector<vector<int>> a, bool diag);

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
	double gt3;
	int sum;
	
	for (unsigned dls_vec_index = 0; dls_vec_index < dls_vec.size(); dls_vec_index++) {
		cout << "dls_vec_index " << dls_vec_index << endl;
		gt1 = cpuTime();
		getTrans(dls_vec[dls_vec_index], isSearchingDiagMates);
		gt2 = cpuTime();
		sum = 0;
		for (unsigned i = 0; i < ltr.size(); i++)
			sum += ltr[i];
		cout << "Generation of " << sum << " transversals took " << gt2 - gt1 << " seconds" << endl;
		int t = getOrt();
		gt3 = cpuTime();
		cout << "Generation of orthogonal pairs took " << gt3 - gt2 << " seconds" << endl;
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

void getTrans(vector<vector<int>> a, bool diag) 
{
	int i;
	int j;
	int k;
	int c;
	int d;
	int t;
	vector<int> h(10);
	vector<bool> x(10);
	
	cout << "sizes of transversals blocks" << endl;
	hb[0] = 0;
	for (t = 0; t < 10; t++) {
		for (k = 0; k < 10; k++) {
			x[k] = false;
		}
		c = 0;
		h[0] = t;
		x[a[0][t]] = true;

		i = 1;
		h[i] = 0;

	label1:
		if (i == 0) goto label4;
		j = h[i];
	label2:
		if (j == 10) {
			i--;
			x[a[i][h[i]]] = false;
			h[i]++;
			goto label1;
		}
		if (x[a[i][j]] == true) {
			j++;
			goto label2;
		}
		for (k = 0; k < i; k++) {
			if (h[k] == j) {
				j++;
				goto label2;
			}
		}

		h[i] = j;
		x[a[i][j]] = true;

		if (i == 9) {
			if (diag == true) {
				d = 0;
				for (k = 0; k < 10; k++) {
					if (k == h[k]) {
						d++;
					}
				}
				if (d != 1)
					goto label3;

				d = 0;
				for (k = 0; k < 10; k++) {
					if (k == h[9 - k]) {
						d++;
					}
				}
				if (d != 1)
					goto label3;
			}
			//cout << "Transversal found!" << endl;
			for (k = 0; k < 10; k++) {
				//		cout << h[k] << " ";
				tr[hb[t] + c][k] = h[k];
			}
			//	cout << endl;
			c++;
		}

	label3:
		if (i == 9)
		{
			x[a[i][h[i]]] = false;
			j++;
			goto label2;
		}
		i++;
		h[i] = 0;
		goto label1;

	label4:
		ltr[t] = c;
		cout << c << endl;
		hb[t + 1] = hb[t] + c;
	}
}

int getOrt() 
{
	int i;
	int ii;
	int k;
	int l;
	vector<int> h(11);
	vector<int> hb(11);
	vector<vector<int>> b(10, vector<int>(10));
	int c;

	hb[0] = 0;
	c = 0;
	for (k = 1; k <= 10; k++) {
		hb[k] = hb[k - 1] + ltr[k - 1];
	}
	i = 0;
	h[0] = 0;

label1:
	//{i - ый блок от hb[i] до hb[i + 1] - 1, h[i] - рассматриваемая тр.}
	//h - это массив индексов трансверсалей, которые образуют текущее разбиение
	ii = h[i];
label2:

	if (ii >= hb[i + 1]) {
		if (i == 0) {
			cout << c << " orthogonal mates" << endl;
			return c;
		}
		//cout << endl << "exceeded block " << i;
		i--;
		h[i]++;
		goto label1;
	}
	//{сравнение с предыдущими трансверсалями}
	for (k = 0; k < i; k++) {
		for (l = 0; l < 10; l++) {
			if (tr[h[k]][l] == tr[ii][l]) {
				ii++;
				goto label2;
			}
		}
	}
	h[i] = ii;
	if (i == 9) {
		for (k = 0; k < 10; k++) {
			for (l = 0; l < 10; l++) {
				b[l][tr[h[k]][l]] = k;
			}
		}
		c++;
		vector<vector<int>> a = b;
		orts[c] = a;
		//cout << c << endl;
		h[i]++;
		goto label1;
	}
	i++;
	h[i] = hb[i];
	goto label1;
}
