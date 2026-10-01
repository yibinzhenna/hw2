#include <iomanip>
#include "mydatastore.h"
#include "util.h"

using namespace std;

MyDataStore::MyDataStore() {}

MyDataStore::~MyDataStore() {
  for (size_t i = 0; i < products_.size(); i++) {
    delete products_[i];
  }

  map<string, User*>::iterator it;
  for (it = users_.begin(); it != users_.end(); ++it) {
    delete it->second;
  }

}

void MyDataStore::addProduct(Product* p) {
  products_.push_back(p);
  set<string> kw = p->keywords();
  set<string>::iterator it;
  for(it = kw.begin(); it != kw.end(); ++it) {
    keywordMap_[convToLower(*it)].insert(p);
  }
}

void MyDataStore::addUser(User* u) {
  string name = convToLower(u->getName());
  users_[name] = u;
  carts_[name];
}

vector<Product*> MyDataStore::search(vector<string>& terms, int type) {
  vector<Product*> hits;
  if (terms.empty()) {
    return hits;
  }

  set<Product*> results;
  for (size_t i = 0; i < terms.size(); i++) {
    set<Product*> matches;
    map<string, set<Product*> >::iterator it = keywordMap_.find(convToLower(terms[i]));
    if (it != keywordMap_.end()) {
      matches = it->second;
    }

    if (i == 0) {
      results = matches;
    }

    else if (type == 0) {
      results = setIntersection(results, matches);
    }

    else {
      results = setUnion(results, matches);
    }
  }
  set<Product*>::iterator sit;
  for(sit = results.begin(); sit != results.end(); ++sit) {
    hits.push_back(*sit);
  }
  return hits;
}

void MyDataStore::dump(ostream& ofile) {
  ofile << "<products>" << endl;
  for (size_t i = 0; i < products_.size(); i++) {
    products_[i]->dump(ofile);
  }
  ofile << "</products>" << endl;

  ofile << "<users>" << endl;
  map<string, User*>::iterator it;
  for (it = users_.begin(); it != users_.end(); ++it) {
    it->second->dump(ofile);
  }
  ofile << "</users>" << endl;
}

bool MyDataStore::addToCart(string username, Product* p) {
  username = convToLower(username);
  if (users_.find(username) == users_.end()) {
    return false;
  }
  carts_[username].push_back(p);
  return true;
}

bool MyDataStore::viewCart(string username) {
  username = convToLower(username);
  if(users_.find(username) == users_.end()) {
    return false;
  }
  vector<Product*>& cart = carts_[username];
  for (size_t i = 0; i < cart.size(); i++) {
    cout << "Item " << setw(3) << (i+1) << endl;
    cout << cart[i]->displayString() << endl;
    cout << endl;
  }
  return true;
}

bool MyDataStore::buyCart(string username) {
  username = convToLower(username);
  map<string, User*>::iterator uit = users_.find(username);
  if (uit == users_.end()) {
    return false;
  }
  User* u = uit->second;
  vector<Product*>& cart = carts_[username];
  vector<Product*> remaining;

  for (size_t i = 0; i < cart.size(); i++) {
    Product* p = cart[i];
    if (p->getQty() > 0 && u->getBalance() >= p->getPrice()) {
      p->subtractQty(1);
      u->deductAmount(p->getPrice());
    }
    else {
      remaining.push_back(p);
    }
  }
  cart = remaining;
  return true;
}




