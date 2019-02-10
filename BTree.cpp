#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using std::string;
using std::vector;

using std::cout;
using std::endl;
using std::cerr;
using std::stringstream;

const int MAX_NODE_INDEX = 4;
const int MIN_NODE_INDEX = 2 * MAX_NODE_INDEX;

//        key_vec[0]               key_vec[1]               key_vec[2]
//       /        \                /      \                /       \
//     /            \            /          \            /           \
//   /                \        /              \        /               \
// branch_vec[0]     branch_vec[1]           branch_vec[2]            branch_vec[3]
struct BTreeNode {
	vector<string> branch_vec;
	vector<string> key_vec;
	BTreeNode(const vector<int> & branch_vec, const vector<int> & key_vec);
};

struct SearchRes {
	// if index is -1 - it's not possible to continue the search
	int index; bool found; BTreeNode * node;
	SearchRes( int i, bool f, BTreeNode * n) : 
		index(i), found(f), node (n)
	{ }
	string print() {
		stringstream ss;
		ss << "index: " << index
			<< ", found: " << found
			<< ", node: " << node;
		return ss.str();
	}
};

SearchRes search_node(string key, BTreeNode * n)
{
	return SearchRes(-1, true, (BTreeNode*)1);
}

SearchRes search(string key, BTreeNode * n)
{
	if (n == 0) { return SearchRes(-1, false, 0); }
	else {
		SearchRes res = search_node(key, n);
		return res;
	}
}

bool unit_test_search_null_node_should_return_impossible_to_continue()
{
	string fn(__PRETTY_FUNCTION__); 

	BTreeNode * node = 0;
	SearchRes res = search("str", node);
	bool test_res = res.found == false && res.index == -1 && res.node == 0;
	if (!test_res) {
		cout << fn << " failed " << "res.found == false && res.index == -1 && res.node == 0"
			<<  " test_res:" << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

int main()
{
	int n_tests = 0, n_passed = 0;
	{ 
		++n_tests;
		unit_test_search_null_node_should_return_impossible_to_continue() ? 
			++n_passed : n_passed;
	}
	cout << " n_tests: " << n_tests << ", n_passed: " << n_passed
		<<  " / " << n_tests << endl;
}
