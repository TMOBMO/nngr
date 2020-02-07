// A class to solve Nonogram (Hadje) Puzzles
// Nigel Galloway - January 23rd., 2017
// https://rosettacode.org/wiki/Nonogram_solver

#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include <bitset>
#include <functional>
#include <numeric>  


using namespace std;

typedef unsigned long long usize;


template<usize _N, usize _M> class Nonogram {
	enum class ng_val : char { X = '#', B = '.', V = '?' };

	template<usize LEN> struct Line {
		Line() {}
		Line(vector<int> len_set, const int size) : X(), B(), Tx(), Tb(), len_set_(len_set), total_(), size_(size) {}

		bitset<LEN> X, B, T, Tx, Tb;
		vector<int> len_set_;
		int total_, size_;

		inline bool fillLen(const int idx, const int len, const bool b) {
			for (int i = idx; i < idx + len; ++i) {
				// check X and B. 
				// X has 1, the element assure always true.
				// B has 1, the element assure always false.
				if ((b && XBV(i) == ng_val::B) || (!b && XBV(i) == ng_val::X))
					return false;
				else
					T[i] = b;
			}
			return true;
		}

		void        fillSet(const int idx, const int min_len, const int line_idx, const int offset) {
			const int len = len_set_[idx];

			if (fillLen(line_idx, offset, false) && fillLen(line_idx + offset, len, true)) {
				if ((idx + 1) < len_set_.size()) {
					if (fillLen(line_idx + len + offset, 1, false))
						fillSet(idx + 1, min_len - len - 1, line_idx + len + offset + 1, 0);
				}
				else {
					// fill and check false after the last piece.
					if (fillLen(line_idx + len + offset, size_ - (line_idx + len + offset), false)) {
						// to find B
						Tb &= T.flip();

						// to find X
						Tx &= T.flip();
						++total_;
					}
				}
			}

			if (offset <= size_ - line_idx - min_len - 1)
				fillSet(idx, min_len, line_idx, offset + 1);
		}

		void        setAssurance(const int idx, const bool b) {
			X.set(idx, b);
			B.set(idx, !b);
		}

		ng_val      XBV(const int idx) const {
			return (X.test(idx)) ? ng_val::X : (B.test(idx)) ? ng_val::B : ng_val::V;
		}

		int         fillOneLine() {
			// only one way is found.
			if (total_ == 1)
				return 1;

			Tx.set();
			Tb.set();
			total_ = 0;

			int s = accumulate(len_set_.cbegin(), len_set_.cend(), 0);
			fillSet(0, s + len_set_.size() - 1, 0, 0);
			return total_;
		}
	}; // end of N

	vector<Line<_M> > rows_;
	vector<Line<_N> > cols_;
	int prev_total_, rows_size_, cols_size_;
	const char* cX, * cB, * cV;

	void setCell(usize y, usize x, bool b) {
		rows_[y].setAssurance(x, b);
		cols_[x].setAssurance(y, b);
	}

public:
	Nonogram(const vector<vector<int>>& rows, const vector<vector<int>>& cols)
		: rows_(), cols_(), prev_total_(), rows_size_(rows.size()), cols_size_(cols.size()),
		cX("бс"), cB("бр"), cV("г┐")
	{
		for (int i = 0; i < cols_size_; i++)
			cols_.push_back(Line<_N>(cols[i], rows_size_));

		for (int i = 0; i < rows_size_; i++)
			rows_.push_back(Line<_M>(rows[i], cols_size_));
	}

	//     Nonogram(const vector<vector<int>>& rows, const vector<vector<int>>& cols, const vector<string>& g)
	//         : rows_{}, cols_{}, En{}, rows_size_(rows.size()), cols_size_(cols.size())
	//     {
	//         for (int i = 0; i < cols_size_; i++)
	//             cols_.push_back(N<_N>(cols[i], rows_size_));
	// 
	//         for (int i = 0; i < rows_size_; i++) {
	//             rows_.push_back(N<_G>(rows[i], cols_size_));
	//             if (i < g.size())
	//             for (int e = 0; e < cols_size_ || e < g[i].size(); e++)
	//             if (g[i][e] == '#') 
	//                 setCell(i, e, true);
	//         }
	//     }

	bool solve() {
		int total = 0;

		for (int i = 0; i < rows_size_; i++) {
			int num;
			if ((num = rows_[i].fillOneLine()) == 0)
				return false;
			else
				total += num;

			for (int j = 0; j < cols_size_; j++)
				if (rows_[i].Tx[j] != rows_[i].Tb[j])
					setCell(i, j, rows_[i].Tx[j]);
		}

#ifdef _DEBUG
		cout << "\n" << toStr() << endl;
#endif

		for (int j = 0; j < cols_size_; j++) {
			int num;
			if ((num = cols_[j].fillOneLine()) == 0)
				return false;
			else
				total += num;

			for (int i = 0; i < rows_size_; i++)
				if (cols_[j].Tx[i] != cols_[j].Tb[i])
					setCell(i, j, cols_[j].Tx[i]);
		}

#ifdef _DEBUG
		cout << "\n" << toStr() << endl;
#endif

		if (total == prev_total_)
			return false;
		else
			prev_total_ = total;

		// only one case remain in each row and column.
		if (total == rows_size_ + cols_size_)
			return true;

		return solve();
	}

	const char* XBV(ng_val val) const {
		if (val == ng_val::X)
			return cX;
		if (val == ng_val::B)
			return cB;
		return cV;
	}

	const string toStr() const {
		ostringstream out;
		for (int i = 0; i < rows_size_; i++) {
			for (int j = 0; j < cols_size_; j++) {
				if (j % 5 == 0)
					out << ' ';
				//out << static_cast<char>(rows_[i].XBV(j));
				out << XBV(rows_[i].XBV(j));

			}
			out << endl;
			if (i % 5 == 4)
				out << endl;
		}
		return out.str();
	}
};


// For the purpose of this task I provide a little code to read from a file in the required format
// Note though that Nonograms may contain blank lines and values greater than 24
int main() {
	ifstream in("in.txt");
	if (in.is_open() == false) {
		cerr << "Unable to open in.txt.\n";
		exit(EXIT_FAILURE);
	}

	string s;
	getline(in, s);

	istringstream iss(s);
	int N, M;
	iss >> N;
	iss >> M;

	vector<vector<int>> rows;
	vector<vector<int>> cols;

	for (int i = 0; i < N; ++i) {
		getline(in, s);
		istringstream iss(s);
		vector<int> row;

		while (iss) {
			string t;
			iss >> t;
			if (t == "")
				continue;
			row.push_back(stoi(t));
		}
		rows.push_back(row);
	}

	for (int i = 0; i < M; ++i) {
		getline(in, s);
		istringstream iss(s);
		vector<int> col;

		while (iss) {
			string t;
			iss >> t;
			if (t == "")
				continue;
			col.push_back(stoi(t));
		}
		cols.push_back(col);
	}


	Nonogram<64, 64> myN(rows, cols);
	if (!myN.solve()) cout << "I don't believe that this is a nonogram!" << endl;
#ifndef _DEBUG
	cout << "\n" << myN.toStr() << endl;
#endif

	cout << "press Enter to exit...";
	getchar();
	return 0;
}