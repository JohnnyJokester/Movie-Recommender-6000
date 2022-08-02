#include <iostream>
#include <fstream>
#include "minHeap.cpp"
#include "rbt.cpp"
using namespace std;

int main() {
    cout << endl << endl;
    // MIN HEAP IMPLEMENTATION //
    auto startHeap = chrono::high_resolution_clock::now();

    ifstream movieFile;
    movieFile.open("movies.csv");
    ifstream ratingsFile;
    ratingsFile.open("ratings 2.csv");
    minHeap heap = minHeap(58098);
    string temporary = "";
    getline(movieFile, temporary);
    getline(ratingsFile, temporary);

    map<int, vector<pair<int, float>>> tempReviews;
    vector<pair<int, pair<string, float>>> userReviews;

    for (unsigned int i = 0; i < 27753444; i++) {
        string tempUserID = "";
        string tempMovieID = "";
        string tempRating = "";
        string restOfLine = "";

        getline(ratingsFile, tempUserID, ',');
        getline(ratingsFile, tempMovieID, ',');
        getline(ratingsFile, tempRating, ',');
        getline(ratingsFile, restOfLine);

        int userID = stoi(tempUserID);
        int movieID = stoi(tempMovieID);
        float rating = stof(tempRating);


        tempReviews[movieID].push_back(make_pair(userID, rating));
        userReviews.push_back(make_pair(userID, make_pair(tempMovieID, rating)));

    }

    for (unsigned int i = 0; i < 58098; i++) {
        Node2* temp = new Node2;
        getline(movieFile, temp->id, ',');


        getline(movieFile, temp->name, ',');
        string tempName = "";
        if (temp->name.at(0) == '\"') {
            temp->name += ",";
            while (temp->name.at(temp->name.size() - 1) != '\"' && temp->name.find(')') == string::npos) {
                getline(movieFile, tempName, ',');
                temp->name += tempName;
            }
            temp->name = temp->name.substr(1, temp->name.size() - 2);
        }


        temporary = "";
        getline(movieFile,temporary);
        int p = 0;

        while (temporary.find('|') != string::npos) {
            p = temporary.find('|');
            temp->genres.push_back(temporary.substr(0,p));
            temporary = temporary.substr(p + 1);
        }
        if (temporary != "(no genres listed)") {
            temp->genres.push_back(temporary);
        }

        for (unsigned int j = 0; j < tempReviews[stoi(temp->id)].size(); j++) {
            temp->reviews.push_back(make_pair(tempReviews[stoi(temp->id)].at(j).first, tempReviews[stoi(temp->id)].at(j).second));
        }

        heap.insertNode2(temp);
    }

    heap.updateUserReviews(userReviews);

    auto stopHeap = chrono::high_resolution_clock::now();
    auto durationHeap = (stopHeap - startHeap);
    auto timeHeap = chrono::duration_cast<chrono::seconds>(durationHeap);

    cout << "Building the min-heap took " << timeHeap.count() << " seconds!" << endl << endl;

    // RED-BLACK TREE IMPLEMENTATION //
    Rbt movieNames;

    //Read in values from the database
    auto startRbt = chrono::high_resolution_clock::now();
    readIntoRbt("movies.csv", movieNames);
    auto stopRbt = chrono::high_resolution_clock::now();
    auto durationRbt = (stopRbt - startRbt);
    auto timeRbt = chrono::duration_cast<chrono::seconds>(durationRbt);

    cout << "Building the Red-Black Tree took " << timeRbt.count() << " seconds!" << endl << endl;

    //TraversePtrs contains all nodes from the tree
    vector<Node*> traversePtrs = getNodes(movieNames.getRoot(), traversePtrs);


    while(true)
    {
        cout << "Which data structure would you like to use?" << endl << endl;
        cout << "1. Min Heap" << endl << "2. Red-Black Tree" << endl << "3. Both" << endl << "4. Exit Program" << endl << endl;
        string dsOption;
        getline(cin, dsOption);
        int dsOptionNum;
        try
        {
            dsOptionNum = stoi(dsOption);
        }
        catch(exception &e)
        {
            cout << endl << "Please enter a valid number (e.g. \"1\" for option \"1.\")" << endl << endl;
            continue;
        }
        if(dsOptionNum == 4)
        {
            break;
        }
        else if(dsOptionNum < 1 || dsOptionNum > 4)
        {
            cout << endl << "Please enter a valid number (e.g. \"1\" for option \"1.\")" << endl << endl;
            continue;
        }
        cout << endl;
        cout << "Let Us Find a Movie for You!" << endl;
        cout << "***************************************" << endl;
        cout << "1. Get suggestions based on a movie" << endl;
        cout << "2. Get suggestions based on a genre" << endl;
        cout << "3. Get suggestions based on user reviews" << endl;
        cout << "4. Get top 10 movies" << endl;
        cout << "5. Get movies to avoid based on a movie" << endl;
        cout << "6. Get movies to avoid based on a genre" << endl;
        cout << "7. Get movies to avoid based on user reviews" << endl;
        cout << "8. Get bottom 10 movies" << endl;
        cout << "9. Exit program" << endl;
        cout << "***************************************" << endl << endl;
        cout << "Please select an option: ";
        string option;
        getline(cin, option);
        int optionNum;
        try
        {
            optionNum = stoi(option);
        }
        catch(exception &e)
        {
            cout << endl << "Please enter a valid number (e.g. \"1\" for option \"1.\")" << endl << endl;
            continue;
        }
        cout << endl;
        if(optionNum == 1)
        {
            cout << "Enter a movie name and we'll give you some suggestions based on that movie: ";
            string movieName;
            getline(cin, movieName);
            cout << endl;
            cout << "Here are 10 highly rated movies of similar genre to " << movieName << ": " << endl << endl;
            if(dsOptionNum == 1)
                heap.goodMovieGenre(movieName);
            else if(dsOptionNum == 2)
                movieNames.suggestionsBasedOnMovie(movieName, traversePtrs);
            else
            {
                heap.goodMovieGenre(movieName);
                movieNames.suggestionsBasedOnMovie(movieName, traversePtrs);
            }
        }
        else if(optionNum == 2)
        {
            cout << "Enter a genre and we'll give you some suggestions in that genre: ";
            string genre;
            getline(cin, genre);
            cout << endl;
            cout << "Here are the 10 highest rated movies in the " << genre << " genre: " << endl << endl;
            if(dsOptionNum == 1)
            {
                heap.bestGenre(genre);
            }
            else if(dsOptionNum == 2)
            {
                movieNames.suggestionsBasedOnGenre(genre, traversePtrs);
            }
            else
            {
                heap.bestGenre(genre);
                movieNames.suggestionsBasedOnGenre(genre, traversePtrs);
            }
        }
        else if(optionNum == 3)
        {
            cout << "Enter a movie name and we'll give you some movies which were given good reviews by reviewers who liked that movie: ";
            string movieName;
            getline(cin, movieName);
            cout << endl;
            cout << "Here are 10 movies given good reviews by reviewers who enjoyed " << movieName << ": " << endl << endl;
            if(dsOptionNum == 1)
            {
                heap.goodMovieReview(movieName);
            }
            else if(dsOptionNum == 2)
            {
                movieNames.suggestionsBasedOnReviews(movieNames.getRoot(), movieName);
            }
            else
            {
                heap.goodMovieReview(movieName);
                movieNames.suggestionsBasedOnReviews(movieNames.getRoot(), movieName);
            }
        }
        else if(optionNum == 4)
        {
            cout << "Here are the 10 highest rated movies: " << endl << endl;
            if(dsOptionNum == 1)
            {
                heap.topOverall();
            }
            else if(dsOptionNum == 2)
            {
                movieNames.bestOverallMovies(traversePtrs);
            }
            else
            {
                heap.topOverall();
                movieNames.bestOverallMovies(traversePtrs);
            }
        }
        else if(optionNum == 5)
        {
            cout << "Enter a movie name and we'll give you some movies to avoid based on that movie: ";
            string movieName;
            getline(cin, movieName);
            cout << endl;
            cout << "Here are 10 poorly rated movies of similar genre to " << movieName << ": " << endl << endl;
            if(dsOptionNum == 1)
            {
                heap.badMovieGenre(movieName);
            }
            else if(dsOptionNum == 2)
            {
                movieNames.avoidBasedOnMovie(movieName, traversePtrs);
            }
            else
            {
                heap.badMovieGenre(movieName);
                movieNames.avoidBasedOnMovie(movieName, traversePtrs);
            }
        }
        else if(optionNum == 6)
        {
            cout << "Enter a genre and we'll give you some movies to avoid in that genre: ";
            string genre;
            getline(cin, genre);
            cout << endl;
            cout << "Here are the 10 lowest rated movies in the " << genre << " genre: " << endl << endl;
            if(dsOptionNum == 1)
            {
                heap.worstGenre(genre);
            }
            else if(dsOptionNum == 2)
            {
                movieNames.avoidBasedOnGenre(genre, traversePtrs);
            }
            else
            {
                heap.worstGenre(genre);
                movieNames.avoidBasedOnGenre(genre, traversePtrs);
            }
        }
        else if(optionNum == 7)
        {
            cout << "Enter a movie name and we'll give you some movies that were given poor reviews by reviewers who did not like that movie: ";
            string movieName;
            getline(cin, movieName);
            cout << endl;
            cout << "Here are 10 movies given poor reviews by reviewers who disliked " << movieName << ": " << endl << endl;
            if(dsOptionNum == 1)
            {
                heap.badMovieReview(movieName);
            }
            else if(dsOptionNum == 2)
            {
                movieNames.avoidBasedOnReview(movieNames.getRoot(), movieName);
            }
            else
            {
                heap.badMovieReview(movieName);
                movieNames.avoidBasedOnReview(movieNames.getRoot(), movieName);
            }
        }
        else if(optionNum == 8)
        {
            cout << "Here are the 10 lowest rated movies: " << endl << endl;
            if(dsOptionNum == 1)
            {
                heap.worstOverall();
            }
            else if(dsOptionNum == 2)
            {
                movieNames.worstOverallMovies(traversePtrs);
            }
            else
            {
                heap.worstOverall();
                movieNames.worstOverallMovies(traversePtrs);
            }
        }
        else if(optionNum == 9)
        {
            break;
        }
        else
        {
            cout << endl << "Please enter a valid number (e.g. \"1\" for option \"1.\")" << endl << endl;
        }
    }

    return 0;
}