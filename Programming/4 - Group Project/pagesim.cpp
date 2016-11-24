//####################################################//
//      Project 4 - PAGE REPLACEMENT PERFORMANCE      //
//            Alec Braynen, Matthew Kramer            //
//           Nghiem Ngo, and Sterling Price           //
//            COT 4600 - OPERATING SYSTEMS            //
//####################################################//

#include <climits>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int max(vector<int> &vector);
int min(vector<int> &vector);
int search(vector<int> &vector, int value);
void print(vector<int> &vector);
void print(vector<int> &frames, int page);
void print(int misses, int hits);
void FIFO(vector<int> &array, int num_frames);
void LRU(vector<int> &array, int num_frames);
void LFU(vector<int> &array, int num_frames);
void MFU(vector<int> &array, int num_frames);
void OPT(vector<int> &array, int num_frames);

int main(void)
{
	int num_frames;
	int option;
	int length;
	int temp;
	ifstream infile;
	string filename;
	vector<int> array;

	cout << endl << "Please select an input method:" << endl;
	cout << "1. File\n2. User\n3. Random\n0. Quit" << endl << endl << "> ";
	cin >> option;
	cin.ignore();

	switch(option)
	{
		case 0:
		{
			cout << endl;
			return 0;
		}

		case 1:
		{
			cout << endl << "Please enter the file name: ";
			getline(cin, filename);

			infile.open(filename.c_str());

			if( !infile.is_open() )
			{
				cout << "Unable to open \"" << filename << "\"." << endl << endl;
				return -1;
			}

			while ( infile.good() )
			{
				infile >> temp;
				array.push_back(temp);
			}

			infile.close();

			cout << endl << "Please enter the number of frames: ";
			cin >> num_frames;
			cin.ignore();

			print(array);

			break;
		}

		case 2:
		{
			cout << endl << "Please enter the length of the trace: ";
			cin >> length;

			cout << endl;

			for ( int i = 0; i < length; i++ )
			{
				cout << "Please enter page reference #" << i + 1 << ": ";
				cin >> temp;
				cin.ignore();
				array.push_back(temp);
			}

			cout << endl << "Please enter the number of frames: ";
			cin >> num_frames;
			cin.ignore();

			print(array);

			break;
		}

		case 3:
		{
			cout << endl << "Please enter the length of the trace: ";
			cin >> length;

			srand(time(NULL));

			for (int i = 0; i < length; i++)
			{
				int temp = rand() % 10;
				array.push_back(temp);
			}

			cout << endl << "A random memory trace has been generated." << endl;

			print(array);

			cout << endl << "Please enter the number of frames: ";
			cin >> num_frames;
			cin.ignore();

			break;
		}

		default:
		{
			cout << endl << "Invalid option." << endl << endl;
			return -1;
		}
	}

	cout << endl << "Please select an algorithm:" << endl;
	cout << "1. FIFO\n2. LRU\n3. LFU\n4. MFU\n5. OPT\n6. ALL\n0. Quit" << endl << endl << "> ";
	cin >> option;
	cin.ignore();

	switch(option)
	{
		case 0:
		{
			cout << endl;
			return 0;
		}

		case 1:
		{
			FIFO(array, num_frames);
			break;
		}

		case 2:
		{
			LRU(array, num_frames);
			break;
		}

		case 3:
		{
			LFU(array, num_frames);
			break;
		}

		case 4:
		{
			MFU(array, num_frames);
			break;
		}

		case 5:
		{
			OPT(array, num_frames);
			break;
		}

		case 6:
		{
			cout << endl;
			cout << "#########################################" << endl;
			cout << "  First-In First-Out" << endl;
			cout << "#########################################" << endl;
			FIFO(array, num_frames);

			cout << "#########################################" << endl;
			cout << "  Least Recently Used" << endl;
			cout << "#########################################" << endl;
			LRU(array, num_frames);

			cout << "#########################################" << endl;
			cout << "  Least Frequently Used" << endl;
			cout << "#########################################" << endl;
			LFU(array, num_frames);

			cout << "#########################################" << endl;
			cout << "  Most Frequently Used" << endl;
			cout << "#########################################" << endl;
			MFU(array, num_frames);

			cout << "#########################################" << endl;
			cout << "Optimal" << endl;
			cout << "#########################################" << endl;
			OPT(array, num_frames);

			break;
		}

		default:
		{
			cout << endl << "Invalid option." << endl << endl;
			break;
		}
	}

	array.clear();
}

// Find the index of the maximum value in a vector
int max(vector<int> &vector)
{
	int max = INT_MIN;
	int max_idx;

	for( int i = 0; i < vector.size(); i++ )
	{
		if( vector[i] > max )
		{
			max = vector[i];
			max_idx = i;
		}
	}

	return max_idx;
}

// Find the index of the minimum value in a vector
int min(vector<int> &vector)
{
	int min = INT_MAX;
	int min_idx;

	for ( int i = 0; i < vector.size(); i++ )
	{
		if ( vector[i] < min)
		{
			min = vector[i];
			min_idx = i;
		}
	}

	return min_idx;
}

// Search for a given value in a vector
int search(vector<int> &vector, int idx, int value)
{
	for(int i = idx; i < vector.size(); i++)
	{
		if( vector[i] == value ) return i;
	}

	return INT_MAX;
}

// Print the contents of a vector
void print(vector<int> &vector)
{
	cout << endl;

	for ( int i = 0; i < vector.size(); i++ )
	{
		cout << vector[i] << " ";
	}

	cout << endl;
}

// Print the contents of the page table
void print(vector<int> &frames, int page)
{
	cout << setfill(' ') << setw(2) << page << " : ";

	for ( int i = frames.size() - 1; i >= 0; --i )
	{
		if (frames[i] == INT_MAX )
		{
			cout << "X ";
		}
		else
		{
			cout << frames[i] << " ";
		}
	}

	cout << endl;
}

// Print the results of a replacement algorithm
void print(int misses, int hits)
{
	cout << endl << "Page Faults: " << misses << ", Page Hits: " << hits;
	cout << endl << "Miss Ratio: " << ((double)misses / (hits + misses)) * 100 << "%";
	cout << ", Hit Ratio: " << ((double)hits / (hits + misses)) * 100 << "%" << endl << endl;
}

// First-In, First-Out
void FIFO(vector<int> &array, int num_frames)
{
	int misses = 0;
	int hits = 0;

	vector<int> frames (num_frames, INT_MAX);

	cout << endl;

	for ( int i = 0; i < array.size(); i++ )
	{
		int idx = search(frames, 0, array[i]);

		if ( idx == INT_MAX )
		{
			frames.erase(frames.begin());
			frames.push_back(array[i]);
			misses++;
		}
		else
		{
			hits++;
		}

		print(frames, array[i]);
	}

	print(misses, hits);
	frames.clear();
}

// Least Recently Used
void LRU(vector<int> &array, int num_frames)
{
	int misses = 0;
	int hits = 0;

	vector<int> frames (num_frames, INT_MAX);

	cout << endl;

	for ( int i = 0; i < array.size(); i++ )
	{
		int idx = search(frames, 0, array[i]);

		if ( idx == INT_MAX )
		{
			frames.erase(frames.begin());
			frames.push_back(array[i]);
			misses++;
		}
		else
		{
			int temp = frames[idx];
			frames.erase(frames.begin() + idx);
			frames.push_back(temp);
			hits++;
		}

		print(frames, array[i]);
	}

	print(misses, hits);
	frames.clear();
}

// Least Frequently Used
void LFU(vector<int> &array, int num_frames)
{
	int misses = 0;
	int hits = 0;

	vector<int> frames (num_frames, INT_MAX);
	vector<int> counts (num_frames, INT_MIN);

	cout << endl;

	for ( int i = 0; i < array.size(); i++ )
	{
		int idx = search(frames, 0, array[i]);

		if ( idx == INT_MAX )
		{
			frames.erase(frames.begin() + min(counts));
			counts.erase(counts.begin() + min(counts));
			frames.push_back(array[i]);
			counts.push_back(0);
			misses++;
		}
		else
		{
			counts[idx] = counts[idx] + 1;
			hits++;
		}

		print(frames, array[i]);
	}

	print(misses, hits);
	frames.clear();
	counts.clear();
}

// Most Frequently Used
void MFU(vector<int> &array, int num_frames)
{
	int misses = 0;
	int hits = 0;

	vector<int> frames (num_frames, INT_MAX);
	vector<int> counts (num_frames, INT_MAX);

	cout << endl;

	for ( int i = 0; i < array.size(); i++ )
	{
		int idx = search(frames, 0, array[i]);

		if ( idx == INT_MAX )
		{
			frames.erase(frames.begin() + max(counts));
			counts.erase(counts.begin() + max(counts));
			frames.push_back(array[i]);
			counts.push_back(0);
			misses++;
		}
		else
		{
			counts[idx] = counts[idx] + 1;
			hits++;
		}

		print(frames, array[i]);
	}

	print(misses, hits);
	frames.clear();
	counts.clear();
}

// Optimal
void OPT(vector<int> &array, int num_frames)
{
	int misses = 0;
	int hits = 0;

	vector<int> frames (num_frames, INT_MAX);
	vector<int> indices (num_frames, INT_MAX);

	cout << endl;

	for ( int i = 0; i < array.size(); i++ )
	{
		int idx = search(frames, 0, array[i]);

		if ( idx == INT_MAX )
		{
			frames.erase(frames.begin() + max(indices));
			indices.erase(indices.begin() + max(indices));
			frames.push_back(array[i]);
			indices.push_back(search(array, i + 1, array[i]));
			misses++;
		}
		else
		{
			indices[idx] = search(array, i + 1, array[i]);
			hits++;
		}

		print(frames, array[i]);
	}

	print(misses, hits);

	frames.clear();
	indices.clear();
}
