// A program to detect SPAMS using Open hashing and Bayesian filtering
// Requirements : Create atleast 10 good mails and 10 bad mails as good1.txt, good2.txt...bad1.txt,bad2.txt and so on
// Create a file test.txt which contains the mail to be tested for spam

#include<iostream>
#include<fstream>
#include<string.h>
#include<cstdlib>

#define MAX 50		//Maximum string length
#define pnum 10		//size of hash table

using namespace std;

struct node
{
	char str[MAX];
	int count;
	struct node* next;
};				// Each token is stored in this structure.

struct test
{
	char str[MAX];
	float prob;
};				// Each token in test mail is stored in this structure.
	

int value( char[] );			//Result of addition of each character(ASCII) in the string.
void add( struct node **, char[] );		//to add node to the hash table

void itoa( int, char[] );     //integer to string conversion. Required for appending numbers to filenames
int search( struct node *, char[] );	// To search in the hash table

void strlwr( char[] );		//Convert string to lowercase for case insensitive searching.
void sort( struct test[], int ); // to sort structures and to choose the most interesting fifteen tokens.

float max( float a, float b );		//Returns Maximum of two numbers
float min( float a, float b );		//Returns Maximum of two numbers


float prod( struct test[], int );	// Product of probability of the most interesting fifteen tokens.
float minusprod( struct test[], int );	// Product of one minus probability of the most interesting fifteen tokens.

void check( char[], char[]);		// To filter all words, i.e to delete , " " ' ' etc

int main()
{
	fstream in;

	int i, ngood = 10, nbad = 10;	// ngood -> number of good mails and similarly nbad
	int index, val;
	int gcount, bcount;	// g -> count of the word in good mails and similarly b
	int tindex = 0; 		//tindex to keep track of the test structure indices

	float gp, bp; 		//gp -> good probability	bp -> bad probability
	float p; 		// probability of the word
	float combined_pro = 0; 	//combined probability

	struct test t[500];
	
	char bad[15] = "bad";
	char num[5];			//to store the offset number w.r.t file
	char word[MAX], token[MAX];
	struct node *g[pnum], *b[pnum];
	
	for( i = 0; i < pnum; i++ )
		g[i] = b[i] = NULL;	
	

	for( i = 0; i < ngood; i++ )	//scanning good mails
	{
		itoa( i+1, num );	//int converted to a string
		char good[15] = "Input/good";
		strcat( good, num );
		strcat( good, ".txt" );
		in.open( good, ios::in );
				
		if( in.fail() )
			cout << "File not found , invalid path"<< endl;
		else
		{
			while( !in.eof() )
			{
				in >> token;
				check( token, word);
				if( strlen(word) != 0 )
				{
					strlwr(word);
					if( !in.eof() )
					{
						val = value( word );
						index = val % 10;
						add( &g[index], word );
					}
				}
			}
		}
		in.close();
		
	}
		
	for( i = 0; i < nbad; i++ )	//scanning all bad mails
	{
		itoa( i+1, num );	//int converted to a string
		char bad[15] = "Input/bad";
		strcat( bad, num );
		strcat( bad, ".txt" );
		in.open( bad, ios::in );
		
		if( in.fail() )
			cout << "File not found , invalid path"<< endl;
		else
		{
			while( !in.eof() )
			{
				in >> token;
				check( token, word);
				if( strlen(word) != 0 )
				{
					strlwr(word);
					if( !in.eof() )
					{
						val = value( word );
						index = val % 10;
						add( &b[index], word );
					}
				}
			}
		}
		in.close();
		
	}
	
	// SPAM DETECTION
	in.open( "Input/test.txt", ios::in );		
	if( in.fail() )
			cout << "File not found , invalid path"<< endl;
	else
	{
		while( !in.eof() )
		{
			in >> token;
			check( token, word);
			if( strlen(word) != 0 )
			{
				strlwr(word);
				if( !in.eof() )
				{
					val = value( word );
					index = val % 10;
					gcount = search( g[index], word );
					gcount = gcount << 1;		
					bcount = search( b[index], word );
				
					if( gcount+bcount >= 5 )
					{
						bp = min( 1, (float)bcount/nbad );
						gp = min( 1, (float)gcount/ngood );
						p = bp / (bp+gp);
						p = min( 0.99, p );	
						p = max( 0.01, p );	// to make probability in the range 0.01-0.99
						strcpy( t[tindex].str, word );
						t[tindex].prob = p;
						tindex++;
					}
				}
			}
		}
	
		sort( t, tindex);		// to sort structures in descending order
		
		if( tindex > 15 )
			tindex = 15;		//the most interesting fifteen tokens are taken
	
		combined_pro = prod( t, tindex ) / ( prod( t, tindex ) + minusprod( t, tindex ) );
		
		if( combined_pro > 0.9 )
			cout << "It is a spam..." << endl;
		else
			cout << "It is not a spam..." << endl;
	}	
		
	return 0;
}


int value( char word[] )		//Result of addition of each character(ASCII) in the string.
{
	int v = 0, i = 0;
	while( word[i] != '\0' )
	{
		v = v + word[i];
		i++;
	}
	return v;
}


void add( struct node **q, char word[] )		//to add node to the hash table
{
	struct node *temp, *NN, *prev; 		//NN - New node
	int flag = 0;
	temp = *q;
	
	if( *q == NULL )		//No nodes in the index
	{
		NN = new struct node [1];
		strcpy( NN->str, word );
		NN->count = 1;
		NN->next = NULL;
		*q = NN;
	}
	else
	{
		while( temp != NULL )
		{
			if( strcmp( temp->str, word ) == 0 )
			{
				temp->count++;
				flag = 1;	// Word found
				break;
			}
			prev = temp;
			temp = temp->next;
		}
		if( flag != 1 )
		{
			NN = new struct node [1];
			strcpy( NN->str, word );
			NN->count = 1;
			NN->next = NULL;
			prev->next = NN;
		}
	}
}


void itoa( int n, char num[] )		//integer to string conversion. Required for appending numbers to filenames
{
	int index = 1, j = 0, base, len;
	char revn[5];
	
	while( n != 0 )			// to reverse the number
	{
		base = n % 10;
		revn[j] = base + '0';
		n = n / 10;	
		j++;		
	}
	revn[j] = '\0';
	j--;

	len = strlen( revn );
	while( j >= 0 )		//converting it to string
	{
		num[len-j-1] = revn[j];
		j--;
	}
	num[len] = '\0';	
}


int search( struct node *q, char word[] )	// To search in the hash table
{
	while( q != NULL )
	{
		if( strcmp( q->str, word ) == 0 )	//case insensitive comparison
			return q->count;
		q = q->next;
	}
	return 0;
}


void strlwr( char s[] )			//Convert string to lowercase for case insensitive searching.
{
	int i = 0;
	while( s[i] != '\0' )
	{
		if( s[i] >= 'A' && s[i] <= 'Z' )
			s[i] = s[i] + 32;	// 'z' - 'A' = 32
		i++;
	}
}


float min( float a, float b )		//Returns Minimum of two numbers
{
	if( a < b )
		return a;
	else
		return b;
}


float max( float a, float b )		//Returns Maximum of two numbers
{
	if( a > b )
		return a;
	else
		return b;
}


void sort( struct test t[], int n )	// to sort structures and to choose the most interesting fifteen tokens.
{
	int i, j, flag;
	for( j = 1; j <= n-1; j++ )
	{
		flag = 0;
		for( i = 0; i <= n-j-1; i++ )
		{
			if( t[i].prob < t[i+1].prob )
			{
				struct test temp;
				temp = t[i];
				t[i] = t[i+1];
				t[i+1] = temp;
				flag = 1;	//Exchange has taken place
			}
		}
		if( flag == 0 )
			break;			//Items are already sorted
	}
}


float prod( struct test t[], int n )    // Product of probability of the most interesting fifteen tokens.
{
	float p = 1;
	for( int i = 0; i < n; i++ )
		p = p * t[i].prob;
	return p;
}


float minusprod( struct test t[], int n )     // Product of one minus probability of the most interesting fifteen tokens.
{
	float p = 1;
	for( int i = 0; i < n; i++ )
		p = p * ( 1 - t[i].prob );
	return p;
}


void check( char token[], char word[])		// To filter all words, i.e to delete , " " ' ' etc
{
	int i, j = 0;
	for( i = 0; i < strlen(token); i++)
	{
		if( (token[i] >= 'A' && token[i] <= 'Z') || (token[i] >= '0' && token[i] <= '9') || (token[i] >= 'a' && token[i] <= 'z') )
			word[j++] = token[i];
	}
	word[j] = '\0';
}
