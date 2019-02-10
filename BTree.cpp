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
	vector<string> key_vec;
	vector<BTreeNode*> branch_vec;
	BTreeNode( const vector<string> & keys, const vector<BTreeNode *> & branches):
		key_vec(keys), branch_vec(branches)
	{}
};


struct SearchRes {
	// if index is -1 - it's not possible to continue the search
	int index; bool found; BTreeNode * node;
	SearchRes( int i, bool f, BTreeNode * n) : 
		index(i), found(f), node (n)
	{ }
	string print() {
		stringstream ss;
		ss << "\nSearchRes::print() index: " << index
			<< ", found: " << found
			<< ", node: " << node;
		return ss.str();
	}
};

// expects never to be called if n == 0
// also expects that n != 0 then atleast 1 key in the node
SearchRes search_node(string key, BTreeNode * n)
{
	string fn (__PRETTY_FUNCTION__);
	cout << "ENTER : " << fn << endl;
	//return SearchRes(-1, true, (BTreeNode*)1);
	if (key < n->key_vec[0]) {
		return SearchRes(0, false, n->branch_vec[0]);
	} else {
		return SearchRes(-1, true, (BTreeNode * )1 );
	}
	
}

SearchRes search(string key, BTreeNode * n)
{
	string fn (__PRETTY_FUNCTION__);
	cout << "ENTER : " << fn << endl;
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


bool unit_test_search_node_should_return_branch_0()
{
	string fn(__PRETTY_FUNCTION__); 

	vector<string> node_keys ;
	node_keys.push_back("ad");
	node_keys.push_back("ah");
	node_keys.push_back("al");
	node_keys.push_back("ap");
	node_keys.push_back("at");
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 1);
	branches.push_back((BTreeNode*) 2);
	branches.push_back((BTreeNode*) 3);
	branches.push_back((BTreeNode*) 4);
	branches.push_back((BTreeNode*) 5);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("ac", node);
	bool test_res = res.found == false && res.index == 0 && res.node == branches[0];
	if (!test_res) {
		cout << fn << " failed " << "res.found == false && res.index == 0 && res.node == 0"
			<<  " test_res: " << test_res 
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
	{
		++n_tests;
		unit_test_search_node_should_return_branch_0() ?
			++n_passed : n_passed;
	}
	cout << " n_tests: " << n_tests << ", n_passed: " << n_passed
		<<  " / " << n_tests << endl;
}
