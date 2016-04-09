#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;


static inline double cpuTime(void) { return (double)clock() / CLOCKS_PER_SEC; }
/*vector<vector<int>> tr(10000, vector<int>(10)); 
vector<int> ltr(10);
vector<int> hb(11);
vector<vector<vector<int>>> orts(10000);
*/

class DLX_column {
public:
	int size;
	int column_number;
	int row_id;

	DLX_column * Left;
	DLX_column * Right;
	DLX_column * Up;
	DLX_column * Down;
	DLX_column * Column;
};

void cover(DLX_column *&c) {
	//cout << "Covered " << c->column_number << endl;
	c->Right->Left = c->Left;
	c->Left->Right = c->Right;

	DLX_column *i;
	DLX_column *j;
	i = c->Down;
	while (i!= c) {		
		j = i->Right;
		while (j != i) {
			j->Down->Up = j->Up;
			j->Up->Down = j->Down;

			j->Column->size--;
			if (j->Column->size < 0) {
				cout << "We are in deep trouble" << endl;
			}
			j = j->Right;
		}
		i = i->Down;
	}
}

void uncover(DLX_column *&c) {
	//cout << "Uncovered " << c->column_number << endl;
	DLX_column *i;
	DLX_column *j;
	i = c->Up;
	while (i != c) {
		j = i->Left;
		while (j != i) {
			j->Column->size++;

			j->Down->Up = j;
			j->Up->Down = j;			

			j = j->Left;
		}
		i = i->Up;
	}
	c->Right->Left = c;
	c->Left->Right = c;
}



void choose_c(DLX_column &h, DLX_column *&c) {	
	DLX_column * j;

	j = h.Right;	
	int min = j->size;
	c = j;
	while (j!= &h) {
		if (j->size < min) {
			c = j;
			min = j->size;
		}
		j = j->Right;
	}
}

void print_solution(vector<DLX_column*> &ps) {
	cout << endl;
	for (int i = 0; i < ps.size(); i++) {
		cout << ps[i]->row_id << " ";
	}
	cout << endl;
}

void search_limited(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr, bool &cont, int &limit, bool &count_only, int &count) {
	//pd = partial solution
	if (k > 10) {
		cout << "we are in trouble" << endl;

	}
	//	cout << "Search " << k << endl;
	if (cont==true){
		if (h.Right == &h) {
			count++;
			if (count_only == false) {
				vector<int> tmpv;
				for (int i = 0; i < ps.size(); i++) {
					tmpv.push_back(ps[i]->row_id);
				}
				tvr.push_back(tmpv);
			}
			if (count > limit) { cont = false; }
			if (count % 100000 == 0) { cout << count << endl; }
			//print_solution(ps);
		}
		else {
			DLX_column * c = NULL;
			choose_c(h, c);
			//	cout << "picked column " << c->column_number << endl;
			cover(c);
			DLX_column * r = c->Down;
			while ((r != c)&&(cont==true)) {
				ps.push_back(r);
				DLX_column * j;
				j = r->Right;
				while (j != r) {
					cover(j->Column);
					j = j->Right;
				}

				search_limited(k + 1, h, ps, tvr, cont,limit,count_only,count);
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
}



void search(int k, DLX_column &h, vector<DLX_column*> &ps, vector<vector<int>> &tvr) {
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
		DLX_column * r=c->Down;
		while (r!= c) {			
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

void TVSET_TO_DLX(DLX_column &root, vector<vector<int>> & tvset, vector<DLX_column*> & elements) {
	int dimension = tvset[0].size();	
	root.Up = NULL;
	root.Down = NULL;
	root.Column = NULL;
	root.row_id = -1;
	root.size = -1;
	root.column_number= -1;
	elements.push_back(&root);
	vector<DLX_column *> columns;
	DLX_column * lastleft = &root;
	for (int i = 0; i < dimension* dimension; i++) {
		DLX_column *ct;
		ct = new (DLX_column);
		ct->column_number = i;
		ct->Down = ct;
		ct->Up = ct;
		ct->size = 0;
		ct->row_id = 0;
		ct->Column = ct;
		ct->Left= lastleft;
		lastleft->Right = ct;
		lastleft = ct;
		columns.push_back(ct);
		elements.push_back(ct);
	}
	lastleft->Right = &root;
	root.Left = lastleft;

	for (int i = 0; i < tvset.size(); i++) {
		vector<int> curtv = tvset[i];
		vector<DLX_column *> tvrow;
		for (int j = 0; j < curtv.size(); j++) {
			DLX_column *ctve;
			ctve = new (DLX_column);
			//column corresponds to characteristic vector of LS or smth of that kind
			int k = j*dimension + curtv[j];

			ctve->Column = columns[k];
			ctve->Column->size++;
			ctve->Down= columns[k];
			ctve->Up = columns[k]->Up;
			ctve->Up->Down = ctve;
			ctve->Down->Up = ctve;
			ctve->row_id = i;			
			ctve->column_number = k;
			ctve->size=-10;
			elements.push_back(ctve);
			tvrow.push_back(ctve);
		}
		
		for (int j = 0; j < tvrow.size()-1; j++) {
			tvrow[j]->Right = tvrow[j + 1];
			tvrow[j]->Right->Left= tvrow[j];
		}
		tvrow[tvrow.size()-1]->Right = tvrow[0];
		tvrow[0]->Left = tvrow[tvrow.size()-1];
	}
	DLX_column *pr = &root;
	
}

static bool nextij(int &i, int &j, vector<int> &ct, vector<vector<int>> & a, vector<bool>&co, vector<bool> & x) {
	int dim = x.size();
	bool r = false;

	while ((r==false)&&(j < dim)) {
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
	if (i < 0)
	{
		return false;
	}
	else return true;
}

static bool nextij_ext(int &i, int &j, vector<int> &ct, vector<vector<int>> & a, vector<bool>&co, vector<bool> & x, bool &diag, bool &md, bool &ad) {
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
				if (diag == true) {
					if ((i == j) && (md == true)) { md = false; }
					if ((i == dim - j - 1) && (ad == true)) { ad = false; }
				}
			}
		}
		else {
			bool acc = true;
			if (diag==true){
				if ((i == j) && (md == true)) { acc = false; }
				if ((i == j) && (md == false)) { md=true; }
				if ((i == dim-j-1) && (ad == true)) { acc = false; }
				if ((i == dim-j-1) && (ad == false)) { ad = true; }
			}
			if ((acc==true)&&(co[j] == false) && (x[a[i][j]] == false)) {
				co[j] = true;
				x[a[i][j]] = true;
				r = true;
			}
		}
	}
	if (i < 0)
	{
		return false;
	}
	else return true;
}



bool initTrans_ext(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x, bool diag, bool &md, bool &ad) {	
	int i = 0;
	int j = -1;	
	for (int i = 0; i < x.size(); i++) {
		co[i] = false;
		x[i] = false;
		ct[i] = 0;
	}
	
	while ((i<10)&&(nextij_ext(i, j, ct, a, co, x,diag,md,ad) != false)){		
		ct[i] = j;
		i++;
		j = -1;
	}
	if (i == 10) return true;
	else return false;	
}

bool initTrans(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x) {
	int i = 0;
	int j = -1;
	for (int i = 0; i < x.size(); i++) {
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

bool nextTrans(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x) {
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
			if (i == 10) res = true;
		}
		else { 
			res = true; 
			out = false;
		}

	}
	return out;
}

bool nextTrans_ext(vector<vector<int>> & a, vector<int> &ct, vector<bool> & co, vector<bool>& x, bool &diag) {
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
			if ((i == 10) && (diag == true)){
				int md = 0;
				int ad = 0;
				for (int l = 0; l < ct.size(); l++) {
					if (ct[l] == l) {md++;}
					if (ct[l] == ct.size()-l-1) {ad++;}					
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

void printvector(vector<int> t) {
	for (int i = 0; i < t.size(); i++) {
		cout << t[i] << " ";
	}
}

vector<vector<int>> getTrans_mod(vector<vector<int>> a, bool diag) {
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
			for (int l = 0; l < h.size(); l++) {
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
		while (nextTrans_ext(a, h, co, x,diag) == true) {
			res[num]=h;
			num++;		

			
		}
	}
	res.erase(res.begin() + num, res.end());
	return res;
}
	

/*vector<vector<int>> tr(10000, vector<int>(10)); 
vector<int> ltr(10);
vector<int> hb(11);
vector<vector<vector<int>>> orts(10000);*/
void getTrans(vector<vector<int>> a, bool diag, vector<vector<int>> &tr, vector<int> &ltr, vector<int> &hb) {
	int i;
	int j;
	int k;
	int c;
	int d;
	int t;
	vector<int> h(10);
	vector<bool> x(10);
	
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
		if (i==9)
		{
			x[a[i][h[i]]] = false;
			j++;
			goto label2;
		}
		i++;
		h[i]=0;
		goto label1;

	label4:
		ltr[t] = c;
		//cout << c << endl;
		hb[t + 1] = hb[t] + c;
	}
}
/*vector<vector<int>> tr(10000, vector<int>(10));
vector<int> ltr(10);
vector<int> hb(11);
vector<vector<vector<int>>> orts(10000);*/
int getOrt(vector<vector<int>> &trv, vector<int> &ltr, vector<vector<int>>&tr, vector<vector<vector<int>>> &orts) {
	int i;
	int ii;
	int k;
	int l;
	vector<int> h(11);
	vector<int> hb(11);
	vector<vector<int>> b(10,vector<int>(10));
	int c;

	hb[0] = 0;
	c = 0;
	for (k = 1; k <= 10; k++) {
		hb[k] = hb[k - 1] + ltr[k - 1];
	}
	i= 0; 
	h[0] = 0;

label1:
	//{i - ый блок от hb[i] до hb[i + 1] - 1, h[i] - рассматриваемая тр.}
	//h - это массив индексов трансверсалей, которые образуют текущее разбиение
	ii = h[i];
label2:

	if (ii >= hb[i + 1]) {
		if (i == 0) {
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
		trv.push_back(h);
		for (k = 0; k < 10; k++) {
		//	cout << h[k] << " ";
			for (l = 0; l < 10; l++) {
				b[l][tr[h[k]][l]] = k;
			}
		}
	//	cout << endl;
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

int strtoi(string s) {
	int x = atoi(s.c_str());
	return x;
}
string inttostr(int number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
}
void read_pairs_from_file(string filename, vector<vector<vector<int>>> &Squares) {
	ifstream in;
	in.open(filename);
	vector<int> a;
	string s;
	while (in.good()) {
		getline(in, s);
		if (s.length()>2){
			int k = 0;
			bool b = true;
			while (b == true) {
				if (s[k] != ' ') {
					string t = s.substr(k, 1);
					int r = strtoi(t);
					a.push_back(r);
				}
				k++;
				if (k == s.length()) { b = false; }
			}
		}
	}
	in.close();
	
	for (int t = 0; t < (a.size()) / 200; t++) {
		vector<vector<int>> LS1;
		vector<vector<int>> LS2;
		for (int i = 0; i < 10; i++) {
			vector<int> tmp1;
			vector<int> tmp2;
			for (int j = 0; j < 10; j++) {						
				tmp1.push_back(a[t*200+i*20+j]);
				
				tmp2.push_back(a[t * 200 + i * 20 +10 + j]);				
			}
			LS1.push_back(tmp1);
			LS2.push_back(tmp2);
		}
		Squares.push_back(LS1);
		Squares.push_back(LS2);
		/*cout << "LS1" << endl;
		for (int u = 0; u < 10; u++) {
			for (int v = 0; v < 10; v++) {
				cout << LS1[u][v] << " ";
			}
			cout << endl;
		}

		cout << "LS2" << endl;
		for (int u = 0; u < 10; u++) {
			for (int v = 0; v < 10; v++) {
				cout << LS2[u][v] << " ";
			}
			cout << endl;
		}
		*/
	}
	/*
	for (int i = 0; i < Squares.size(); i++) {
		string fn = "D:\\LSTests\\LSD10_square" + inttostr(i);
		ofstream out;
		out.open(fn);
		for (int u = 0; u < 10; u++) {
			for (int v = 0; v < 10; v++) {
				out << Squares[i][u][v] << " ";
			}
			out << "\n";
		}
		out.close();
	}
	*/
}

void read_squares_from_file(string filename, vector<vector<vector<int>>> &Squares) {
	ifstream in;
	in.open(filename);
	vector<int> a;
	string s;
	while (in.good()) {
		getline(in, s);
		if (s.length()>2) {
			int k = 0;
			bool b = true;
			while (b == true) {
				if (s[k] != ' ') {
					string t = s.substr(k, 1);
					int r = strtoi(t);
					a.push_back(r);
				}
				k++;
				if (k == s.length()) { b = false; }
			}
		}
	}
	in.close();

	for (int t = 0; t < (a.size()) / 100; t++) {
		vector<vector<int>> LS1;		
		for (int i = 0; i < 10; i++) {
			vector<int> tmp1;			
			for (int j = 0; j < 10; j++) {
				tmp1.push_back(a[t * 100 + i * 10 + j]);				
			}
			LS1.push_back(tmp1);
			
		}
		Squares.push_back(LS1);		
		/*cout << "LS1" << endl;
		for (int u = 0; u < 10; u++) {
		for (int v = 0; v < 10; v++) {
		cout << LS1[u][v] << " ";
		}
		cout << endl;
		}
		*/		
	}
	/*
	for (int i = 0; i < Squares.size(); i++) {
	string fn = "D:\\LSTests\\LSD10_square" + inttostr(i);
	ofstream out;
	out.open(fn);
	for (int u = 0; u < 10; u++) {
	for (int v = 0; v < 10; v++) {
	out << Squares[i][u][v] << " ";
	}
	out << "\n";
	}
	out.close();
	}
	*/
}


void check_belyaev(vector<vector<int>> SQ,bool diag, string filename) {
	ofstream out;
	out.open(filename, ios::app);
	vector<vector<int>> tr(10000, vector<int>(10));
	vector<int> ltr(10);
	vector<int> hb(11);
	vector<vector<vector<int>>> orts(10000);
	getTrans(SQ, diag,tr,ltr,hb);
	int sum = 0;
	for (int i = 0; i < ltr.size(); i++) {
		sum += ltr[i];
	}
	out << "Total: " << sum << " transversals" << endl;
	vector<vector<int>> trv;
	int t = getOrt(trv,ltr,tr,orts);
	out << "Found " << trv.size() << "sets of disjoint transversals" << endl;
	if (trv.size()>0){
		for (int i = 0; i < trv.size(); i++) {
			for (int j = 0; j < trv[i].size(); j++) {
				out << trv[i][j] << " ";
			}
			out << endl;
		}
	}
	out.close();
}


void check_dlx(vector<vector<int>> SQ, bool diag, string filename) {
	ofstream out;
	out.open(filename, ios::app);	

	vector<vector<int>> trm = getTrans_mod(SQ, diag);
	out << "Total: " << trm.size()<< " transversals" << endl;
	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, trm, elements);
	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	search(0,*root, ps,tvr);
	for (int i = 0; i < tvr.size(); i++) {
		sort(tvr[i].begin(), tvr[i].end());
	}
	
	for (int i = 0; i < elements.size(); i++) {
		delete elements[i];
	}
	out << "Found " << tvr.size() << "sets of disjoint transversals" << endl;
	if (tvr.size()>0) {
		for (int i = 0; i < tvr.size(); i++) {
			for (int j = 0; j < tvr[i].size(); j++) {
				out << tvr[i][j] << " ";
			}
			out << endl;
		}
	}
	out.close();
}

void check_squares_Belyaev(vector<vector<vector<int>>> Squares, bool diag, string logfilename) {
	ofstream out;

	double t_start = cpuTime();
	for (int i = 0; i < Squares.size(); i++) {
		out.open(logfilename, ios::app);
		out << endl << "Square " << i << endl;
		out.close();
		check_belyaev(Squares[i], diag, logfilename);
	}
	double t_end = cpuTime();
	out.open(logfilename, ios::app);
	out << "Total time required to process the set is " << t_end - t_start << " seconds" << endl;
	out.close();
}

void check_squares_DLX(vector<vector<vector<int>>> Squares, bool diag, string logfilename) {
	ofstream out;

	double t_start = cpuTime();
	
	for (int i = 0; i < Squares.size(); i++) {
		if ((i > 0) && (i % 1000 == 0)) { cout << "Checked " << i << " squares" << endl; }
		out.open(logfilename, ios::app);
		out << endl << "Square " << i << endl;
		out.close();
		check_dlx(Squares[i], diag, logfilename);
	}
	double t_end = cpuTime();
	out.open(logfilename, ios::app);
	out << "Total time required to process the set is " << t_end - t_start << " seconds" << endl;
	out.close();
}

void free_dlx(vector<DLX_column*> &elements) {
	for (int i = 0; i < elements.size(); i++) {
		elements[i]->Column = NULL;
		elements[i]->Up= NULL;
		elements[i]->Down= NULL;
		elements[i]->Left= NULL;
		elements[i]->Right= NULL;		
		DLX_column *t = elements[i];
		delete t;
	}
	elements.clear();
}
void generate_permutations(int n, vector<vector<int>> &perm, bool diag) {
	vector<int> seed;
	for (int i = 0; i < n; i++) {
		seed.push_back(i);
	}

	do {
		bool acc = true;
		if (diag == true) {
			int md = 0;
			int ad = 0;
			for (int j = 0; j < n; j++) {
				if (seed[j] == j) { md++; }
				if (seed[j] == n - j - 1) {ad++;}
			}
			if ((md != 1) || (ad != 1)) {acc = false;}
		}
		if (acc == true) { perm.push_back(seed); }
	} while (std::next_permutation(seed.begin(),seed.end()));

}

void construct_square_from_tv( vector<vector<int>> &tv_set, vector<int> &tv_ind, vector<vector<int>> &SQ) {
	
	for (int i = 0; i < tv_ind.size(); i++) {
		for (int j = 0; j < tv_set[tv_ind[i]].size(); j++) {
			SQ[j][tv_set[tv_ind[i]][j]] = i;
		}
	}
}
void construct_squares_from_tv_set(vector<vector<int>>&tv_set, vector<vector<int>> &tv_index_sets, vector<vector<vector<int>>> &SQUARES) {
	for (int i = 0; i < tv_index_sets.size(); i++) {
		construct_square_from_tv(tv_set, tv_index_sets[i], SQUARES[i]);
	}
}
void Generate_ODLS(int limit, string logname) {
	int n = 10;
	vector<vector<int>> perm_diag;	
	generate_permutations(n, perm_diag, true);
	
	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, perm_diag, elements);
	vector<DLX_column*> ps;
	ps.clear();
	vector<vector<int>> tvr;
	bool cont = true;	
	double t1 = cpuTime();
	int count = 0;
	bool count_only = true;
	search_limited(0, *root, ps, tvr,cont,limit,count_only,count);
	double t2 = cpuTime();
	cout << limit << " squares generated in " << t2 - t1 << " seconds" << endl;
	//vector<vector<int>>> SQUARES(10, vector<int>(10));	
	vector<vector<vector<int>>> SQUARES;
	for (int i = 0; i < tvr.size(); i++) {(10, vector<int>(10));	
		vector<vector<int>> SQ(10, vector<int>(10));
		SQUARES.push_back(SQ);
	}
	
	construct_squares_from_tv_set(perm_diag, tvr, SQUARES);
//	check_squares_DLX(SQUARES, true, logname);

}
void main() {
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
	
	//Generate_ODLS(10000000,"D:\\LSTests\\batch1kk_dlx.log");
	/*
	vector<vector<vector<int>>> Squares;
	//read_pairs_from_file("D:\\LSTests\\ODLS_10_pairs.txt", Squares);
	read_squares_from_file("D:\\LSTests\\2_05_fix_sq.txt", Squares);
	
	check_squares_Belyaev(Squares, true, "D:\\LSTests\\02_05_Belyaev.log");

	check_squares_DLX(Squares, true, "D:\\LSTests\\02_05_dlx.log");
	*/
	vector<vector<int>> tr(10000, vector<int>(10));
	vector<int> ltr(10);
	vector<int> hb(11);
	vector<vector<vector<int>>> orts(10000);
	double gt1 = cpuTime();
	getTrans(a, true, tr, ltr, hb);
	double gt2 = cpuTime();

	int sum = 0;

	for (int i = 0; i < ltr.size(); i++) {
		sum += ltr[i];
	}
	cout << "Total: " << sum << " transversals" << endl;
	
	double gt3 = cpuTime();
	vector<vector<int>> trm=getTrans_mod(a, true);

	for (int i = 0; i < trm.size(); i++) {
		if (trm[i] != tr[i]) {
			cout << "Different transversals @ " << i << endl;
		}
	}
	double gt4 = cpuTime();

	cout << "Generation of "<<sum<<" transversals by Belyaev algorithm took " << gt2 - gt1 << "seconds" << endl;
	cout << "Generation of " << trm.size()<< " transversals by NEW algorithm took " << gt4 - gt3 << "seconds" << endl;

	double gt5 = cpuTime();
	DLX_column *root;
	root = new (DLX_column);
	vector<DLX_column*> elements;
	TVSET_TO_DLX(*root, trm, elements);	
	vector<DLX_column*> ps;
	double gt6 = cpuTime();

	cout << "Transformation of transversal set to DLX format took " << gt6 - gt5 << "seconds" << endl;
	vector<vector<int>>trv;
	int t = getOrt(trv,ltr,tr,orts);
	double gt7 = cpuTime();
	cout << "Generation of orthogonal pairs by Belyaev algorithm took " << gt7 - gt6 << "seconds" << endl;
	
	vector<vector<int>>trvdlx;

	search(0,* root, ps,trvdlx);

	free_dlx(elements);
	double gt8 = cpuTime();
	cout << "Generation of orthogonal pairs by DLX took " << gt8 - gt7 << "seconds" << endl;
	
	
	//cout << "Generation of "<<sum<<" transversals by Belyaev algorithm took " << gt2 - gt1 << "seconds" << endl;
	

	//cout << "Generation of " << sum << " transversals by NEW algorithm took " << gt4 - gt3 << "seconds" << endl;
	

	/*int t = getOrt();
	double gt3 = cpuTime();
	cout << "Generation of orthogonal pairs took " << gt3 - gt2 << "seconds" << endl;*/
	cout << endl << "Finish";
	int u;
	cin >> u;
		

}


