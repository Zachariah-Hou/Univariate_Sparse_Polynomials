#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <ctype.h>
#define DEFAULT_SIZE 10

class UnivariateSparsePolynomials
{
private:
	// Declare the node of the doubly linked list
	struct Term
	{
		//static Term* freelist;

		int coef;			// Coefficient of the term
		int expo;			// Exponent of the term

		Term *next, *prev;	// Pointers

		Term(const int item_A, const int item_B, Term* prevp, Term* nextp)
		{
			coef = item_A;
			expo = item_B;
			prev = prevp;
			next = nextp;
		}

		Term(Term* prevp = NULL, Term* nextp = NULL)
		{
			prev = prevp;
			next = nextp;
		}
		/*		// Overloaded new operator
				void* operator new(size_t)
				{
					if (freelist == NULL) return ::new Term;
					Term* temp = freelist;
					freelist = freelist->next;
					return temp;
				}

				// Overloaded delete operator
				void operator delete(void* ptr)
				{
					((Term*)ptr)->next = freelist;
					freelist = (Term*)ptr;
				}*/
		// Reload the operator = to assign polynomials easily
		Term& operator =(Term& term)
		{
			this->coef = term.coef;
			this->expo = term.expo;
			return *this;
		}

	} *curr, *head, *tail;	// Declare the pointero

	// TODO: 完善Freelist

	//Term* Term::freelist = NULL;

	int cnt;				// The length of the list

	// Intialization helper method
	void init()
	{
		curr = head = new Term;
		head->next = tail = new Term(head, NULL);
		cnt = 0;
	}

	// Return link nodes to free stor
	void removeAll()
	{
		while (head != NULL) {
			curr = head;
			head = head->next;
			delete curr;
		}
	}

	// Check and feedback
	void assert(bool val, std::string s)
	{
		if (!val) {
			std::cout << "Assertion Failed: " << s << std::endl;
			exit(-1);
		}
	}


public:
	// Constructor & Destructor
	UnivariateSparsePolynomials() { init(); };
	~UnivariateSparsePolynomials() { removeAll(); };

	void clearTheList() { removeAll(); init(); }		// Clear list
	int getLength() { return cnt; }						// Get the length of the list

	void moveToStart() { curr = head; }					// Place curr at list start
	void moveToEnd() { curr = tail->prev; }				// Place curr at list end

	// Move down list to "pos" position 
	Term& moveToPos(const int pos)
	{
		assert((pos >= 0) && (pos <= cnt), "Position out of range");
		curr = head;
		for (int i = 0; i < pos; i++) curr = curr->next;
		return *curr;
	}

	// Move fence one step right, no change if right is empty
	void next()
	{
		if (curr != tail->prev)
			curr = curr->next;
	}

	// Move fence one step left, no change if left is empty
	void prev()
	{
		if (curr != head)
			curr = curr->prev;
	}

	// Insert items at current position
	void insert(const int& item_A, const int& item_B)
	{
		curr->next = curr->next->prev =
			new Term(item_A, item_B, curr, curr->next);
		cnt++;
	}

	// Append items to the end of the list
	void append(const int& item_A, const int& item_B)
	{
		tail->prev = tail->prev->next =
			new Term(item_A, item_B, tail->prev, tail);
		cnt++;
	}

	// Input the polynomials and save them in list
	void input()
	{
		int coef, expo;
		//std::cout << "Please input a term in the first polynomial: " << std::endl;
		for (int i = 1; ; i++) {
			std::cout << i << ":";
			std::cout << "\tCoefficient:";
			std::cin >> coef;
			//if (coef == '#') {
			//	break;
			//}
			std::cout << "\tExponent:";
			std::cin >> expo;
			append(coef, expo);
			curr = tail->prev;
			std::cout << "\nPress \'N\' to break, or press ENTER to continue.\t";
			if (getchar() == 'N' || getchar() == 'n') {
				break;
			} else {
				continue;
			}
		}
		sortPolynomial(1, getLength());
	}

	// Sort and format the input polynomials
	void sortPolynomial(int low, int high)
	{
		if (low >= high) return;

		int	first = low;
		int last = high;
		Term key = moveToPos(first);

		while (first < last) {
			while (first < last && moveToPos(last).expo >= key.expo) {
				last--;
			}
			moveToPos(first) = moveToPos(last);
			while (first < last && moveToPos(first).expo <= key.expo) {
				first++;
			}
			moveToPos(last) = moveToPos(first);
		}

		moveToPos(first) = key;
		sortPolynomial(low, first - 1);
		sortPolynomial(first + 1, high);
	}

	// Reload operator + to complete the addition between two polynomials
	UnivariateSparsePolynomials& operator +(UnivariateSparsePolynomials& poly)
	{
		//this->sortPolynomial(1, getLength());
		//poly.sortPolynomial(1, getLength());
		int totalLength = this->getLength() > poly.getLength() ?
			this->getLength() : poly.getLength();
		this->moveToStart(); poly.moveToStart();
		while (this->curr->next != this->tail || poly.curr->next != poly.tail) {
			if (poly.curr->next == poly.tail) {
				this->next();
			} else if (this->curr->next == this->tail) {
				this->insert(poly.curr->next->coef, poly.curr->next->expo);
				poly.next();
			} else if (poly.curr->next->expo < this->curr->next->expo) {
				this->insert(poly.curr->next->coef, poly.curr->next->expo);
				poly.next();
			} else if (poly.curr->next->expo == this->curr->next->expo) {
				this->curr->next->coef += poly.curr->next->coef;
				this->next(); poly.next();
			} else {
				this->next();
			}
		}
		this->sortPolynomial(1, this->getLength());
		return *this;
	}

	// Reload operator << to output the final polynomials
	friend std::ostream& operator <<(std::ostream& os, UnivariateSparsePolynomials& poly)
	{
		std::string plusSign = "",	// （真的是不会英文了= =）使第一项的系数为正的时候，项前没有正号
			minusSign = " - ";
		poly.moveToStart();
		for (int i = 0; i < poly.getLength(); i++) {
			if (poly.curr->next->expo == 0) {					// 考虑指数为0的情况
				if (poly.curr->next->coef < 0) {
					os << minusSign << poly.curr->next->coef;
					poly.next();
				} else if (poly.curr->next->coef == 0) {
					poly.next();
				} else {
					os << plusSign << poly.curr->next->coef;
					poly.next();
				}
			} else if (poly.curr->next->expo == 1) {			// 考虑指数为1的情况
				if (poly.curr->next->coef < -1) {
					os << minusSign << poly.curr->next->coef << "x";
					poly.next();
				} else if (poly.curr->next->coef == -1) {
					os << minusSign << "x";
				} else if (poly.curr->next->coef == 0) {
					poly.next();
				} else if (poly.curr->next->coef == 1) {
					os << plusSign << "x";
					poly.next();
				} else {
					os << plusSign << poly.curr->next->coef << "x";
					poly.next();
				}
			} else {											// 考虑指数为其他的情况
				if (poly.curr->next->coef < -1) {
					os << minusSign << poly.curr->next->coef << "x^" << poly.curr->next->expo;
					poly.next();
				} else if (poly.curr->next->coef == -1) {
					os << minusSign << "x^" << poly.curr->next->expo;
					poly.next();
				} else if (poly.curr->next->coef == 0) {
					poly.next();
				} else if (poly.curr->next->coef == 1) {
					os << plusSign << "x^" << poly.curr->next->expo;
					poly.next();
				} else {
					os << plusSign << poly.curr->next->coef << "x^" << poly.curr->next->expo;
					poly.next();
				}
			}
			if (i == 0) {
				plusSign = " + ";	// （真的是不会英文了= =）使第一项之后的系数为正的项前加上正号
			}
		}
		return os;
	}
};


int main()
{
	using namespace std;
	UnivariateSparsePolynomials poly_A, poly_B;
	cout << "Please input a term in the first polynomial: \n" << endl;
	poly_A.input();
	//cout << poly_A << endl;
	cout << "\nPlease input a term in the second polynomial: \n" << endl;
	poly_B.input();
	cout << "\nThe polynomial A is:\n\n\t\t" << poly_A << endl 
		<< "\nThe polynomial B is:\n\n\t\t" << poly_B << endl;
	cout << "\nThe result of A+B is:\n\n\t\t" << poly_A + poly_B << endl;
	return 0;
}