#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

using std::string;
using std::vector;

using std::cout;
using std::endl;
using std::cerr;
using std::stringstream;

const int MAX_NODE_INDEX = 3;
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
	BTreeNode() : key_vec(), branch_vec() { }
	bool is_leaf() { return branch_vec.size() == 0; }
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
// return value: if found the index is a key_vec index
// return value: if NOT found the index is a branch index
SearchRes search_node(string key, BTreeNode * n)
{
	string fn (__PRETTY_FUNCTION__);
	cout << "ENTER : " << fn << endl;
	//return SearchRes(-1, true, (BTreeNode*)1);
	if (key < n->key_vec[0]) {
		cout << fn << " path 1" << endl;
		return SearchRes(0, false, n->branch_vec[0]);
	} else if (key > n->key_vec[n->key_vec.size() - 1]) {
		// an absurd value
		// return SearchRes(-1, true, (BTreeNode * )1 );
		cout << fn << " path 2" << endl;
		return SearchRes(n->key_vec.size(), false,
				n->branch_vec[n->key_vec.size()]);
	} else {
		int i = n->key_vec.size() - 1;
		while (i > 0 && (key < n->key_vec[i])) { --i; }
		cout << fn << " path 3: i: " << i << endl;
		if (key == n->key_vec[i]) {
			// NOTE we are returning a key_vec index
			return SearchRes(i, true, n );
		} else {
			// NOTE we are returning a branch_vec index
			return SearchRes(i+1, false, n->branch_vec[i+1] );
		}
	}
}

struct InsertResult {
	bool node_was_split;
	string median_key_from_below;
	vector<string> new_left_keys;
	vector<string> new_right_keys;
	BTreeNode* new_right_child_of_median_key;
	BTreeNode * new_root;
	InsertResult(bool f, BTreeNode * p_new_root): node_was_split(f), new_root(p_new_root) { }
	string print() {
		string fn(__PRETTY_FUNCTION__);
		stringstream ss;
		ss << fn << " node_was_split: " << node_was_split
			<< ", median_key_from_below: " << median_key_from_below << endl;
		for (int i = 0; i < new_root->key_vec.size(); ++i) {
			ss << ", key_vec[" << i << "]: " << new_root->key_vec[i];
		}
		ss << endl;
		return ss.str();
	}
};

void print_vec(string label, vector<string> v)
{
	cout << label << "sz: " << v.size() << endl;
	for (int i = 0; i < v.size(); ++i) { cout <<  ", " <<  v[i]; }
	cout << endl;
}

int find_insert_position(string key, BTreeNode * n)
{

	int i = 0;
	for (; i < n->key_vec.size(); ++i) {
		//cout << "INFO " << fn << ", i: " << i 
		//	<< ", key: " << key << ", key_vec[" << i << "]: "
		//	<< n->key_vec[i]
		//	<< ", (key < n->key_vec[i]): " << (key < n->key_vec[i])
		//	<< endl;
		if (key < n->key_vec[i]) {
			break;
		}
	}
	return i;

}

void insert_into_non_full_node(string key, BTreeNode * n, BTreeNode * right_branch)
{
	string fn(__PRETTY_FUNCTION__);
	cout << "ENTER " << fn << ", key : " << key << ", right_branch: " << right_branch << endl;
	print_vec(fn, n->key_vec);

	//int i = 0;
	//for (; i < n->key_vec.size(); ++i) {
	//	cout << "INFO " << fn << ", i: " << i 
	//		<< ", key: " << key << ", key_vec[" << i << "]: "
	//		<< n->key_vec[i]
	//		<< ", (key < n->key_vec[i]): " << (key < n->key_vec[i])
	//		<< endl;
	//	if (key < n->key_vec[i]) {
	//		break;
	//	}
	//}
	int insert_pos = find_insert_position(key, n);

	cout << "INFO " << fn << ", i: " << insert_pos << endl;
	n->key_vec.insert(n->key_vec.begin() + insert_pos  ,  key);
}

InsertResult  insert_into_full_node_and_split(string key, BTreeNode * n, BTreeNode * right_branch)
{
	string fn (__PRETTY_FUNCTION__);
	// return dummy value for Now
	int insert_pos = find_insert_position(key, n);
	if (insert_pos < MAX_NODE_INDEX) {
		// new node going into the left half
		n->key_vec.insert(n->key_vec.begin() + insert_pos  ,  key);
		vector<string> v_left, v_right;
		string median_key = n->key_vec[(MAX_NODE_INDEX + 1)/2];
		v_left.reserve(MAX_NODE_INDEX+1); v_right.reserve(MAX_NODE_INDEX+1);
		std::copy_n(n->key_vec.cbegin(), (MAX_NODE_INDEX+1)/2 , v_left.begin());
		std::copy_n(n->key_vec.cbegin()+ (MAX_NODE_INDEX+1)/2 + 1,
				(MAX_NODE_INDEX+1)/2, v_right.begin());
		cout << "median_key: " << median_key << endl;
		print_vec("v_left", v_left);
		print_vec("v_right", v_right);
	} else {
		// new node going into the right half
	}
	return InsertResult(false, n);
}

// assumption - this fn will never be called with root == NULL
InsertResult recursivelyInsertAtLeaf(string key, BTreeNode * & n)
{
	string fn (__PRETTY_FUNCTION__);
	cout << "ENTER " << fn << key << endl;
	if (n->is_leaf()) {
		// if there's space insert the data into the leaf
		if (n->key_vec.size() < MAX_NODE_INDEX + 1) {
			insert_into_non_full_node(key, n, 0);
			return InsertResult(false, n);
		} else {
			// we need to insert the key and split the 
			// node at the median
			InsertResult  res = insert_into_full_node_and_split(key, n, 0);
			// for now as a place holder return a dummy wrong value for 
			// code compilation sake
			// In Idris, you can leave a hole.
			return res;
		}
	} else {
		// split the leaf and pass the median key up
		// the parent for insertion 
		// right subtrees of the median key
		// will go into the right sibling of this node
		// the immediate right child of the median will be the 
		// new subtree that was split off
		// NOTE for now - dummy value 
		return InsertResult(true, n);
	}
}

InsertResult  insert(string key, BTreeNode * & root)
{
	if (root == 0) {
		root = new BTreeNode();
		root->key_vec.push_back(key);
		return InsertResult(false, root);
	} else {
		 return recursivelyInsertAtLeaf(key, root);
	}
}

SearchRes search(string key, BTreeNode * n)
{
	string fn (__PRETTY_FUNCTION__);
	cout << "ENTER : " << fn << endl;
	if (n == 0) { return SearchRes(-1, false, 0); }
	else {
		SearchRes res = search_node(key, n);
		if (res.found) {
			return res;
		} else {
			return search (key, n->branch_vec[res.index]);
		}
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
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("ac", node);
	bool test_res = res.found == false && res.index == -1 && res.node == 0;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == false && res.index == 0 && res.node == 0"	
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_should_find_key_0()
{
	string fn(__PRETTY_FUNCTION__); 
	vector<string> node_keys ;
	node_keys.push_back("ad");
	node_keys.push_back("ah");
	node_keys.push_back("al");
	node_keys.push_back("ap");
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 1);
	branches.push_back((BTreeNode*) 2);
	branches.push_back((BTreeNode*) 3);
	branches.push_back((BTreeNode*) 4);
	branches.push_back((BTreeNode*) 5);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("ad", node);
	bool test_res = res.found == true && res.index == 0 && res.node == node;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == true && res.index == 0 && res.node == 0"	
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_should_return_branch_1()
{
	string fn(__PRETTY_FUNCTION__); 
	vector<string> node_keys ;
	node_keys.push_back("ad");
	node_keys.push_back("ah");
	node_keys.push_back("al");
	node_keys.push_back("ap");
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("af", node);
	bool test_res = res.found == false && res.index == -1 && res.node == 0;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == false && res.index == 1 && res.node == branches[1]"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}


bool unit_test_search_node_should_find_key_1()
{
	string fn(__PRETTY_FUNCTION__); 
	vector<string> node_keys ;
	node_keys.push_back("ad");
	node_keys.push_back("ah");
	node_keys.push_back("al");
	node_keys.push_back("ap");
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 1);
	branches.push_back((BTreeNode*) 2);
	branches.push_back((BTreeNode*) 3);
	branches.push_back((BTreeNode*) 4);
	branches.push_back((BTreeNode*) 5);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("ah", node);
	bool test_res = res.found == true && res.index == 1 && res.node == node;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == true && res.index == 1 && res.node == 0"	
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_should_return_branch_2()
{
	string fn(__PRETTY_FUNCTION__); 
	vector<string> node_keys ;
	node_keys.push_back("ad");
	node_keys.push_back("ah");
	node_keys.push_back("al");
	node_keys.push_back("ap");
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("aj", node);
	bool test_res = res.found == false && res.index == -1 && res.node == 0;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == false && res.index == -1 && res.node == 0"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_should_find_key_2()
{
	string fn(__PRETTY_FUNCTION__); 
	vector<string> node_keys ;
	node_keys.push_back("ad");
	node_keys.push_back("ah");
	node_keys.push_back("al");
	node_keys.push_back("ap");
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 1);
	branches.push_back((BTreeNode*) 2);
	branches.push_back((BTreeNode*) 3);
	branches.push_back((BTreeNode*) 4);
	branches.push_back((BTreeNode*) 5);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("al", node);
	bool test_res = res.found == true && res.index == 2 && res.node == node;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == true && res.index == 2 && res.node == 0;"	
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_should_return_branch_3()
{
	string fn(__PRETTY_FUNCTION__); 
	vector<string> node_keys ;
	node_keys.push_back("ad");
	node_keys.push_back("ah");
	node_keys.push_back("al");
	node_keys.push_back("ap");
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("an", node);
	bool test_res = res.found == false && res.index == -1 && res.node == 0;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == false && res.index == -1 && res.node == 0"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_should_find_key_3()
{
	string fn(__PRETTY_FUNCTION__); 
	vector<string> node_keys ;
	node_keys.push_back("ad");
	node_keys.push_back("ah");
	node_keys.push_back("al");
	node_keys.push_back("ap");
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 1);
	branches.push_back((BTreeNode*) 2);
	branches.push_back((BTreeNode*) 3);
	branches.push_back((BTreeNode*) 4);
	branches.push_back((BTreeNode*) 5);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("ap", node);
	bool test_res = res.found == true && res.index == 3 && res.node == node;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == true && res.index == 3 && res.node == 0"	
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_should_return_branch_4()
{
	string fn(__PRETTY_FUNCTION__); 
	vector<string> node_keys ;
	node_keys.push_back("ad");
	node_keys.push_back("ah");
	node_keys.push_back("al");
	node_keys.push_back("ap");
	vector<BTreeNode*> branches;
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	branches.push_back((BTreeNode*) 0);
	BTreeNode * node = new BTreeNode(node_keys, branches);
	SearchRes res = search("ar", node);
	bool test_res = res.found == false && res.index == -1 && res.node == 0;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == false && res.index == -1 && res.node == 0"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

BTreeNode * make_lev0_lev1_data()
{

	BTreeNode * n0_lev0 = 0;
	{
		vector<string> node_keys ;
		node_keys.push_back("ad");
		node_keys.push_back("ah");
		node_keys.push_back("al");
		node_keys.push_back("ap");
		vector<BTreeNode*> branches;
		branches.push_back((BTreeNode*) 1);
		branches.push_back((BTreeNode*) 2);
		branches.push_back((BTreeNode*) 3);
		branches.push_back((BTreeNode*) 4);
		branches.push_back((BTreeNode*) 5);
		n0_lev0 = new BTreeNode(node_keys, branches);
	}

	BTreeNode * n0_lev1 = 0;
	{
		vector<string> node_keys ;
		node_keys.push_back("abd");
		node_keys.push_back("abh");
		node_keys.push_back("abl");
		node_keys.push_back("abp");
		vector<BTreeNode*> branches;
		branches.push_back((BTreeNode*) 11);
		branches.push_back((BTreeNode*) 12);
		branches.push_back((BTreeNode*) 13);
		branches.push_back((BTreeNode*) 14);
		branches.push_back((BTreeNode*) 15);
		n0_lev1 = new BTreeNode(node_keys, branches);
	}
	n0_lev0->branch_vec[0] = n0_lev1;

	BTreeNode * n1_lev1 = 0;
	{
		vector<string> node_keys ;
		node_keys.push_back("afd");
		node_keys.push_back("afh");
		node_keys.push_back("afl");
		node_keys.push_back("afp");
		vector<BTreeNode*> branches;
		branches.push_back((BTreeNode*) 21);
		branches.push_back((BTreeNode*) 22);
		branches.push_back((BTreeNode*) 23);
		branches.push_back((BTreeNode*) 24);
		branches.push_back((BTreeNode*) 25);
		n1_lev1 = new BTreeNode(node_keys, branches);
	}
	n0_lev0->branch_vec[1] = n1_lev1;

	BTreeNode * n2_lev1 = 0;
	{
		vector<string> node_keys ;
		node_keys.push_back("ajd");
		node_keys.push_back("ajh");
		node_keys.push_back("ajl");
		node_keys.push_back("ajp");
		vector<BTreeNode*> branches;
		branches.push_back((BTreeNode*) 31);
		branches.push_back((BTreeNode*) 32);
		branches.push_back((BTreeNode*) 33);
		branches.push_back((BTreeNode*) 34);
		branches.push_back((BTreeNode*) 35);
		n2_lev1 = new BTreeNode(node_keys, branches);
	}
	n0_lev0->branch_vec[2] = n2_lev1;

	BTreeNode * n3_lev1 = 0;
	{
		vector<string> node_keys ;
		node_keys.push_back("and");
		node_keys.push_back("anh");
		node_keys.push_back("anl");
		node_keys.push_back("anp");
		vector<BTreeNode*> branches;
		branches.push_back((BTreeNode*) 41);
		branches.push_back((BTreeNode*) 42);
		branches.push_back((BTreeNode*) 43);
		branches.push_back((BTreeNode*) 44);
		branches.push_back((BTreeNode*) 45);
		n3_lev1 = new BTreeNode(node_keys, branches);
	}
	n0_lev0->branch_vec[3] = n3_lev1;

	BTreeNode * n4_lev1 = 0;
	{
		vector<string> node_keys ;
		node_keys.push_back("asd");
		node_keys.push_back("ash");
		node_keys.push_back("asl");
		node_keys.push_back("asp");
		vector<BTreeNode*> branches;
		branches.push_back((BTreeNode*) 51);
		branches.push_back((BTreeNode*) 52);
		branches.push_back((BTreeNode*) 53);
		branches.push_back((BTreeNode*) 54);
		branches.push_back((BTreeNode*) 55);
		n4_lev1 = new BTreeNode(node_keys, branches);
	}
	n0_lev0->branch_vec[4] = n4_lev1;

	return n0_lev0;

}

bool unit_test_search_node_lev_1_take_br0_0()
{
	string fn(__PRETTY_FUNCTION__); 
	BTreeNode * n0_lev0 = make_lev0_lev1_data();
	SearchRes res = search("abd", n0_lev0);
	bool test_res = res.found == true && res.index == 0 && res.node == n0_lev0->branch_vec[0];
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == true && res.index == 0 && res.node == n0_lev0->branch_vec[0]"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_lev_1_take_br0_3()
{
	string fn(__PRETTY_FUNCTION__); 
	BTreeNode * n0_lev0 = make_lev0_lev1_data();
	SearchRes res = search("abp", n0_lev0);
	bool test_res = res.found == true && res.index == 3 && res.node == n0_lev0->branch_vec[0];
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == true && res.index == 0 && res.node == n0_lev0->branch_vec[0]"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_lev_1_take_br0_2()
{
	string fn(__PRETTY_FUNCTION__); 
	BTreeNode * n0_lev0 = make_lev0_lev1_data();
	SearchRes res = search("abl", n0_lev0);
	bool test_res = res.found == true && res.index == 2 && res.node == n0_lev0->branch_vec[0];
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == true && res.index == 0 && res.node == n0_lev0->branch_vec[0]"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}


bool unit_test_search_node_lev_1_take_br4_2()
{
	string fn(__PRETTY_FUNCTION__); 
	BTreeNode * n0_lev0 = make_lev0_lev1_data();
	SearchRes res = search("asl", n0_lev0);
	bool test_res = res.found == true && res.index == 2 && res.node == n0_lev0->branch_vec[4];
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == true && res.index == 2 && res.node == n0_lev0->branch_vec[4]"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_search_node_lev_1_take_br3_1()
{
	string fn(__PRETTY_FUNCTION__); 
	BTreeNode * n0_lev0 = make_lev0_lev1_data();
	SearchRes res = search("anh", n0_lev0);
	bool test_res = res.found == true && res.index == 1 && res.node == n0_lev0->branch_vec[3];
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == true && res.index == 1 && res.node == n0_lev0->branch_vec[3]"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_insert_null_root()
{
	string fn(__PRETTY_FUNCTION__); 
	BTreeNode * r = 0;
	string k = "k";
	InsertResult  res = insert(k, r); 
	bool test_res = res.node_was_split == false && r->key_vec[0] == k && r->key_vec.size() == 1;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.node_was_split == false && r->key_vec[0] == k && r->key_vec.size() == 1"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_insert_1()
{
	string fn(__PRETTY_FUNCTION__); 
	BTreeNode * n = new BTreeNode();
	n->key_vec.push_back("b");
	string k = "a";
	InsertResult  res = insert(k, n); 
	bool test_res = 
		res.node_was_split == false && 
		n->key_vec.size() == 2 &&
		n->key_vec[0] == k &&
		n->key_vec[1] == "b"
		;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.node_was_split == false && r->key_vec[0] == k && r->key_vec.size() == 1"
			<< "res.node_was_split == false && "
			<< "r->key_vec[0] == k && r->vector[1] == \"b\" &&"
			<< "r->key_vec.size() == 2"  << endl
			<<  " test_res: " << test_res 
			<< endl
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_insert_2()
{
	string fn(__PRETTY_FUNCTION__); 
	BTreeNode * n = new BTreeNode();
	n->key_vec.push_back("a");
	string k = "b";
	InsertResult  res = insert(k, n); 
	bool test_res = 
		res.node_was_split == false && 
		n->key_vec.size() == 2 &&
		n->key_vec[0] == "a" &&
		n->key_vec[1] == "b" 
		;
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.node_was_split == false && r->key_vec[0] == k && r->key_vec.size() == 1"
			<< "res.node_was_split == false && "
			<< "r->key_vec[0] == \"a\" && r->vector[1] == \"b\" &&"
			<< "r->key_vec.size() == 2"  << endl
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_insert_31()
{
	string fn(__PRETTY_FUNCTION__); 
	BTreeNode * n = new BTreeNode();
	n->key_vec.push_back("a");
	n->key_vec.push_back("b");
	string k = "c";
	InsertResult  res = insert(k, n); 
	bool test_res = 
		res.node_was_split == false && 
		n->key_vec.size() == 3 &&
		n->key_vec[0] == "a" &&
		n->key_vec[1] == "b" &&
		n->key_vec[2] == "c"
		;
	if (!test_res) {
		cout << fn << " failed " 
			<<  " test_res: " << test_res 
			<< " res.node_was_split == false && "
			<< " n->key_vec[0] == \"a\" &&"
			<< " n->key_vec[1] == \"b\" &&"
			<< " n->key_vec[2] == \"c\" &&"
			<< " n->key_vec.size() == 3 "
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_insert_32()
{
	string fn(__PRETTY_FUNCTION__); 
	cout << "ENTER " << fn << endl;
	BTreeNode * n = new BTreeNode();
	n->key_vec.push_back("b");
	n->key_vec.push_back("c");
	string k = "a";
	InsertResult  res = insert(k, n); 
	bool test_res = 
		res.node_was_split == false && 
		n->key_vec.size() == 3 &&
		n->key_vec[0] == "a" &&
		n->key_vec[1] == "b" &&
		n->key_vec[2] == "c";
	if (!test_res) {
		cout << fn << " failed " 
			<<  " test_res: " << test_res 
			<< " res.node_was_split == false && "
			<< " n->key_vec[0] == \"a\" &&"
			<< " n->key_vec[1] == \"b\" &&"
			<< " n->key_vec[2] == \"c\" &&"
			<< " n->key_vec.size() == 3 "
			<< res.print()
			<< endl;
	}
	return test_res;
}

bool unit_test_insert_into_full_node_and_split_1()
{
	return false;
}

/*
bool unit_test_search_node_should_return_branch_5()
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
	SearchRes res = search("av", node);
	bool test_res = res.found == false && res.index == 5 && res.node == branches[5];
	if (!test_res) {
		cout << fn << " failed " 
			<< "res.found == false && res.index == 5 && res.node == branches[5]"
			<<  " test_res: " << test_res 
			<< res.print()
			<< endl;
	}
	return test_res;
}
*/

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
	{
		++n_tests;
		unit_test_search_node_should_return_branch_1() ?
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_should_return_branch_2() ?
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_should_return_branch_3() ?
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_should_return_branch_4() ?
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_should_find_key_0() ?
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_should_find_key_1() ?
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_should_find_key_2() ?
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_should_find_key_3() ?
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_lev_1_take_br0_0() ? 
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_lev_1_take_br0_3() ? 
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_lev_1_take_br0_2() ? 
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_lev_1_take_br4_2() ? 
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_search_node_lev_1_take_br3_1() ? 
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_insert_null_root() ? 
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_insert_1() ? 
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_insert_2() ? 
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_insert_31() ? 
			++n_passed : n_passed;
	}
	{
		++n_tests;
		unit_test_insert_32() ? 
			++n_passed : n_passed;
	}

	//{
	//	++n_tests;
	//	unit_test_search_node_should_return_branch_5() ?
	//		++n_passed : n_passed;
	//}
	cout << " n_tests: " << n_tests << ", n_passed: " << n_passed
		<<  " / " << n_tests << endl;
}
