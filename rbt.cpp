#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <iomanip>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <chrono>

using namespace std;

enum Color {RED, BLACK};

//-----------------------------START OF RBT IMPLEMENTATION-------------------------------//
// Class to store movie objects
struct Node
{
    string movieName; // movie title
    int id; // movie id
    vector<pair<int, float>> reviews; // all reviews associated with movie
    vector<string> genres; // all genres associated with movie
    Node *left;
    Node *right;
    Node *parent;
    bool color;

    //Overloaded Constructor
    Node(string& movieName, int id, vector<pair<int,float>>& reviews, vector<string>& genres)
    {
        this->movieName = movieName;
        this->id = id;
        this->reviews = reviews;
        this->genres = genres;
        this->left = nullptr;
        this->right = nullptr;
        this->parent = nullptr;
        this->color = BLACK;
    }
};

class Rbt
{
private:
    Node* root;
    vector<pair<int, pair<int, float>>> reviewsByUser;

    void rotateRight(Node* &root, Node* &node);
    void rotateLeft(Node* &root, Node* &node);
    void fixViolation(Node* &root, Node* &node);

public:
    Rbt()
    {
        this->root = nullptr;
    }

    void insertNode(string& movieName, int id, vector<pair<int,float>>& reviews, vector<string>& genres);

    //HelperFunctions
    Node* searchMovie(Node* root, string name);
    Node* searchMovieID(Node* root, int id);
    Node* getRoot();
    void updateReviewsByUser(vector<pair<int, pair<int, float>>> vec);

    //Method calls
    void suggestionsBasedOnMovie(string name, vector<Node*>& traversePtrs); //Option 1
    void suggestionsBasedOnGenre(string genre, vector<Node*>& traversePtrs); //Option 2
    void suggestionsBasedOnReviews(Node* root, string movie); //Option 3
    void bestOverallMovies(vector<Node*>& traversePtrs); //Option 4
    void avoidBasedOnMovie(string name, vector<Node*>& traversePtrs); //Option 5 (Inverse of 1)
    void avoidBasedOnGenre(string genre, vector<Node*>& traversePtrs); //Option 6 (Inverse of 2)
    void avoidBasedOnReview(Node* root, string movie); //Option 7 (Inverse of 3)
    void worstOverallMovies(vector<Node*>& traversePtrs); //Option 8 (Inverse of 4)
};

//-------------------RED BLACK TREE FUNCTIONS----------------------//
void Rbt::insertNode(string& movieName, int id, vector<pair<int,float>>& reviews, vector<string>& genres)
{

    // create a new node object initialized to necessary values
    Node* node = new Node(movieName, id,reviews,genres);


    // create temporary nodes to help with insertion
    Node* y = nullptr;
    Node* x = root;

    // as long as x isn't nullptr, find its place sequentially in the tree
    while (x != nullptr) {
        y = x;
        if (node->movieName < x->movieName) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }

    // set y as the node's parent
    node->parent = y;

    // if the node has no parent, it's the root
    if (y == nullptr) {
        root = node;
    }
        // if the node is less than its parent, move it left
    else if (node->movieName < y->movieName) {
        y->left = node;
    }
        // otherwise, move it right
    else {
        y->right = node;
    }

    // if the node has no parent, color the node black and don't rotate anything
    if (node->parent == nullptr) {
        node->color = BLACK;
        return;
    }
    // if the node has no grandparent, don't rotate anything
    if (node->parent->parent == nullptr) {
        return;
    }

    //If there are any violations of the Red-Black Tree, fix it
    fixViolation(root, node);
}

//Rotates the tree to the right
void Rbt::rotateRight(Node* &root, Node* &node)
{
    //Variables
    Node* node_left = node->left;
    node->left = node_left->right;

    // if there is a left subtree, fix parent of subtree
    if (node->left != nullptr)
    {
        node_left->right->parent = node;
    }
    node_left->parent = node->parent;

    // if the node has no parent, set the root equal to left node
    if (node->parent == nullptr)
    {
        root = node_left;
    }

        // otherwise, if the node is parent's right child, set it equal to the left node
    else if (node == node->parent->right)
    {
        node->parent->right = node_left;
    }

        // otherwise, set left child equal to node's left
    else
    {
        node->parent->left = node_left;
    }

    // set parent and left node's right node
    node_left->right = node;
    node->parent = node_left;
}

//Rotates the tree to the left
void Rbt::rotateLeft(Node *&root, Node *&node)
{
    //Variables
    Node* node_right = node->right;
    node->right = node_right->left;

    // if there is a right-left child, set right-left's parent equal to node
    if (node_right->left != nullptr)
    {
        node_right->left->parent = node;
    }

    node_right->parent = node->parent;

    // if the node has no parent, the root is the node's right child
    if (node->parent == nullptr)
    {
        root = node_right;
    }
        // if the node is left child, set left child equal to node's right child
    else if (node->parent->left == node)
    {
        node->parent->left = node_right;
    }
        // otherwise, set parent's right child equal to node's right child
    else
    {
        node->parent->right = node_right;
    }

    // set node's right-left child and node's parent to correct nodes
    node_right->left = node;
    node->parent = node_right;
}

void Rbt::fixViolation(Node *&root, Node *&node)
{
    // create a temporary node to track switching nodes
    Node* temp;

    // while the node's parent is red, keep looping
    while (node->parent->color == RED) {

        // if the node's parent is the right child, enter here
        if (node->parent == node->parent->parent->right) {
            // set temp node equal to grandparent's left child
            temp = node->parent->parent->left;

            // if that node is red, set it equal to black, change parent to black, and grandparent to red
            if (temp->color == RED) {
                temp->color = BLACK;
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            }
            else {
                // otherwise, if node is left child, set it equal to its parent and rotate right
                if (node == node->parent->left) {
                    node = node->parent;
                    rotateRight(root, node);
                }
                // set parent equal to black and grandparent equal to red, and rotate left
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rotateLeft(root, node->parent->parent);
            }
        }
        else {
            // set node equal to grandparent's right child
            temp = node->parent->parent->right;

            // if node is red, set equal to black, change parent to black, and set grandparent equal to red
            if (temp->color == RED)
            {
                temp->color = BLACK;
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            }
            else
            {
                // if node is right child, set it equal to parent and rotate left
                if (node == node->parent->right)
                {
                    node = node->parent;
                    rotateLeft(root, node);
                }

                // set parent to black, set grandparent to red, nad rotate right on node's grandparent
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rotateRight(root, node->parent->parent);
            }
        }
        // if the node is the root, don't rotate anymore
        if (node == root)
        {
            break;
        }
    }
    // root is always black
    root->color = BLACK;
}

float calculateAverageRatings(vector<pair<int, float>> reviews)
{
    //Variables
    float result = 0.0;

    for (int i = 0; i < reviews.size(); i++)
    {
        result += reviews[i].second;
    }

    return result / (float)reviews.size();
}

//OPTION 1
void Rbt::suggestionsBasedOnMovie(string name, vector<Node*>& traversePtrs)
{
    //Clock to keep track
    auto start = chrono::high_resolution_clock::now();

    //Variables
    Node* movieNode = searchMovie(root, name);
    vector<pair<string, float>> namesOfMovies;
    set<pair<string,vector<pair<int,float>>>> similarMovieGenres;
    bool found = true;

    if (movieNode != nullptr)
    {
        //Finds movies with similar genres and add it to a set
        for (int i = 0; i < movieNode->genres.size(); i++)
        {
            //Iterate through EVERY MOVIE
            for (int j = 0; j < traversePtrs.size(); j++)
            {
                //Check if the movie is not the one inputted
                if (traversePtrs[j]->movieName != name)
                {
                    //Iterates through the reviews of the current MOVIE
                    for (int k = 0; k < traversePtrs[j]->genres.size(); k++)
                    {
                        if (movieNode->genres[i] == traversePtrs[j]->genres[k] && traversePtrs[j]->reviews.size() > 100)
                        {
                            //Puts it in a set so no copies of movies are made
                            similarMovieGenres.emplace(make_pair(traversePtrs[j]->movieName, traversePtrs[j]->reviews));
                        }
                    }
                }
            }
        }

        //Transfers from set to a vector that only contains movies that have more than 100 reviews
        for (auto it = similarMovieGenres.begin(); it != similarMovieGenres.end(); it++)
        {
            if (it->second.size() > 100)
            {
                namesOfMovies.emplace_back(make_pair(it->first, calculateAverageRatings(it->second)));
            }
        }

        sort(namesOfMovies.begin(),namesOfMovies.end(), [] (const auto &movie1, const auto &movie2) {return movie1.second > movie2.second;});

        //Print out top 10 movies
        if (namesOfMovies.size() < 10) {
            for (unsigned int i = 0; i < namesOfMovies.size(); i++) {
                float rating = round(namesOfMovies[i].second * 10.0) / 10.0;
                cout <<  i + 1 << ". " << namesOfMovies[i].first << ", Rating: " << fixed << setprecision(1) << rating <<endl;
            }
        }
        else {
            for (int i = 0; i < 10; i++) {
                float rating = round(namesOfMovies[i].second * 10.0) / 10.0;
                cout << i + 1 << ". " << namesOfMovies[i].first << ", Rating: " << fixed << setprecision(1) << rating
                     << endl;
            }
        }

        //Print the time it took to complete
        auto stop = chrono::high_resolution_clock::now();
        auto duration = (stop - start);
        auto time = chrono::duration_cast<chrono::milliseconds>(duration);

        cout << endl << "This process took " << time.count() << " milliseconds using a Red-Black Tree!" << endl << endl;
    }
    else
    {
        cout << "Invalid Movie! Please enter a valid movie title" << endl << endl;
        return;
    }
}

void Rbt::avoidBasedOnMovie(string name, vector<Node*>& traversePtrs)
{
    //Clock to keep track
    auto start = chrono::high_resolution_clock::now();

    //Variables
    Node* movieNode = searchMovie(root, name);
    vector<pair<string, float>> namesOfMovies;
    set<pair<string,vector<pair<int,float>>>> similarMovieGenres;
    bool found = true;

    if (movieNode != nullptr)
    {
        //Finds movies with similar genres and add it to a set
        for (int i = 0; i < movieNode->genres.size(); i++)
        {
            //Iterate through EVERY MOVIE
            for (int j = 0; j < traversePtrs.size(); j++)
            {
                //Check if the movie is not the one inputted
                if (traversePtrs[j]->movieName != name)
                {
                    //Iterates through the reviews of the current MOVIE
                    for (int k = 0; k < traversePtrs[j]->genres.size(); k++)
                    {
                        if (movieNode->genres[i] == traversePtrs[j]->genres[k] && traversePtrs[j]->reviews.size() > 100)
                        {
                            //Puts it in a set so no copies of movies are made
                            similarMovieGenres.emplace(make_pair(traversePtrs[j]->movieName, traversePtrs[j]->reviews));
                        }
                    }
                }
            }
        }

        //Transfers from set to a vector that only contains movies that have more than 100 reviews
        for (auto it = similarMovieGenres.begin(); it != similarMovieGenres.end(); it++)
        {
            if (it->second.size() > 100)
            {
                namesOfMovies.emplace_back(make_pair(it->first, calculateAverageRatings(it->second)));
            }
        }

        sort(namesOfMovies.begin(),namesOfMovies.end(), [] (const auto &movie1, const auto &movie2) {return movie1.second < movie2.second;});

        //Print out top 10 movies
        if (namesOfMovies.size() < 10) {
            for (unsigned int i = 0; i < namesOfMovies.size(); i++) {
                float rating = round(namesOfMovies[i].second * 10.0) / 10.0;
                cout <<  i + 1 << ". " << namesOfMovies[i].first << ", Rating: " << fixed << setprecision(1) << rating <<endl;
            }
        }
        else {
            for (int i = 0; i < 10; i++) {
                float rating = round(namesOfMovies[i].second * 10.0) / 10.0;
                cout << i + 1 << ". " << namesOfMovies[i].first << ", Rating: " << fixed << setprecision(1) << rating
                     << endl;
            }
        }

        //Print the time it took to complete
        auto stop = chrono::high_resolution_clock::now();
        auto duration = (stop - start);
        auto time = chrono::duration_cast<chrono::milliseconds>(duration);

        cout << endl << "This process took " << time.count() << " milliseconds using a Red-Black Tree!" << endl << endl;
    }
    else
    {
        cout << "Invalid Movie! Please enter a valid movie title" << endl << endl;
        return;
    }
}

Node* Rbt::searchMovie(Node* root, string movie)
{
    //If the movie is found or was not found
    if (root == nullptr || root->movieName == movie)
    {
        return root;
    }

    //Recursively iterate through the left side of the tree to find if that movie exists
    if (movie < root->movieName)
    {
        return searchMovie(root->left, movie);
    }

    //Recursively iterate through the left side of the tree to find if that movie exists
    return searchMovie(root->right, movie);
}

Node* Rbt::searchMovieID(Node* root, int id)
{
    // if this node is the wanted node, return it
    if (root->id == id)
    {
        return root;
    }

    Node* temp = nullptr;
    // if there is a left child, search there for node
    if (root->left != nullptr)
    {
        temp = searchMovieID(root->left, id);
    }
    // if node was found, return it
    if (temp != nullptr)
    {
        return temp;
    }
    // if there is a right child, search there for node
    if (root->right != nullptr)
    {
        temp = searchMovieID(root->right, id);
    }
    // return temp (nullptr if not found, points to a node otherwise)
    return temp;
}

Node *Rbt::getRoot()
{
    //Returns the root of the tree
    return this->root;
}

void Rbt::suggestionsBasedOnGenre(string genre, vector<Node*>& traversePtrs)
{
    //Clock to keep track
    auto start = chrono::high_resolution_clock::now();

    //Variables
    bool genreFound = false;
    vector<pair<string,float>> moviesList; //pair<movieName, AverageRating>

    //Iterate through all the movies and find the ones that have a genre inputted
    for (int i = 0; i < traversePtrs.size(); i++)
    {
        for (int k = 0; k < traversePtrs[i]->genres.size(); k++)
        {
            if (genre == traversePtrs[i]->genres[k] && traversePtrs[i]->reviews.size() > 100)
            {
                genreFound = true;
                moviesList.emplace_back(make_pair(traversePtrs[i]->movieName, calculateAverageRatings(traversePtrs[i]->reviews)));
                break;
            }
        }
    }

    if (genreFound)
    {
        //Sort the movies
        sort(moviesList.begin(),moviesList.end(), [] (const auto &movie1, const auto &movie2) {return movie1.second > movie2.second;});
    }
    else
    {
        cout << "Invalid Genre! Please enter a valid genre" << endl << endl;
        return;
    }

    //Print out top 10 movies
    if (moviesList.size() < 10) {
        for (unsigned int i = 0; i < moviesList.size(); i++) {
            float rating = round(moviesList[i].second * 10.0) / 10.0;
            cout <<  i + 1 << ". " << moviesList[i].first << ", Rating: " << fixed << setprecision(1) << rating <<endl;
        }
    }
    else {
        for (int i = 0; i < 10; i++) {
            float rating = round(moviesList[i].second * 10.0) / 10.0;
            cout << i + 1 << ". " << moviesList[i].first << ", Rating: " << fixed << setprecision(1) << rating << endl;
        }
    }

    //Print the time it took to complete
    auto stop = chrono::high_resolution_clock::now();
    auto duration = (stop - start);
    auto time = chrono::duration_cast<chrono::milliseconds>(duration);

    cout << endl << "This process took " << time.count() << " milliseconds using a Red-Black Tree!" << endl << endl;
}

void Rbt::updateReviewsByUser(vector<pair<int, pair<int, float>>> vec)
{
    reviewsByUser = vec;
}

void Rbt::avoidBasedOnGenre(string genre, vector<Node *> &traversePtrs)
{
    //Clock to keep track
    auto start = chrono::high_resolution_clock::now();

    //Variables
    bool genreFound = false;
    vector<pair<string,float>> moviesList; //pair<movieName, AverageRating>

    //Iterate through all the movies and find the ones that have a genre inputted
    for (int i = 0; i < traversePtrs.size(); i++)
    {
        for (int k = 0; k < traversePtrs[i]->genres.size(); k++)
        {
            if (genre == traversePtrs[i]->genres[k] && traversePtrs[i]->reviews.size() > 100)
            {
                genreFound = true;
                moviesList.emplace_back(make_pair(traversePtrs[i]->movieName, calculateAverageRatings(traversePtrs[i]->reviews)));
                break;
            }
        }
    }

    //If the genre is found
    if (genreFound)
    {
        sort(moviesList.begin(),moviesList.end(), [] (const auto &movie1, const auto &movie2) {return movie1.second < movie2.second;});
    }
    else
    {
        cout << "Invalid Genre! Please enter a valid genre" << endl << endl;
        return;
    }

    //Print out top 10 movies
    if (moviesList.size() < 10) {
        for (unsigned int i = 0; i < moviesList.size(); i++) {
            float rating = round(moviesList[i].second * 10.0) / 10.0;
            cout <<  i + 1 << ". " << moviesList[i].first << ", Rating: " << fixed << setprecision(1) << rating <<endl;
        }
    }
    else {
        for (int i = 0; i < 10; i++) {
            float rating = round(moviesList[i].second * 10.0) / 10.0;
            cout << i + 1 << ". " << moviesList[i].first << ", Rating: " << fixed << setprecision(1) << rating << endl;
        }
    }

    //Print the time it took to complete
    auto stop = chrono::high_resolution_clock::now();
    auto duration = (stop - start);
    auto time = chrono::duration_cast<chrono::milliseconds>(duration);

    cout << endl << "This process took " << time.count() << " milliseconds using a Red-Black Tree!" << endl << endl;
}

void Rbt::suggestionsBasedOnReviews(Node *root, string movie)
{
    //Clock to keep track of time
    auto start = chrono::high_resolution_clock::now();

    // find the movie that the user input
    Node* temp = searchMovie(root, movie);

    // if movie wasn't found, print error message
    if (temp == nullptr)
    {
        cout << "Invalid Movie! Please enter a valid movie title" << endl << endl;
        return;
    }

    // intialize a vector for the highest reviews
    vector<pair<float, int>> highReviews;

    // search all the reviews of the movie
    for (unsigned int i = 0; i < temp->reviews.size(); i++)
    {
        // if review is greater than or equal to 4.0, push it into vector
        if (temp->reviews.at(i).second >= 4.0)
        {
            highReviews.push_back(make_pair(temp->reviews.at(i).second, temp->reviews.at(i).first));
        }
    }


    // sort reviews from highest to lowest
    sort(highReviews.begin(), highReviews.end(), greater<>());

    vector<pair<float, int>> temporary;

    // if there are more than 100, take only 100 highest reviews
    if (highReviews.size() > 100)
    {
        for (unsigned int k = 0; k < 100; k++)
        {
            temporary.push_back(make_pair(highReviews.at(k).first, highReviews.at(k).second));
        }
        highReviews.clear();
        highReviews = temporary;
        temporary.clear();
    }

    vector<pair<int, float>> reviewsByTitle;

    // for every high rating
    for (unsigned int h = 0; h < highReviews.size(); h++)
    {
        // for all the reviews sorted by user
        for (unsigned int l = 0; l < reviewsByUser.size(); l++)
        {
            // if the same user gave both ratings, the movie is different, and the rating is 4.0 or higher, push it into the vector
            if (reviewsByUser.at(l).first == highReviews.at(h).second && reviewsByUser.at(l).second.first != temp->id && reviewsByUser.at(l).second.second >= 4.0)
            {
                reviewsByTitle.push_back(make_pair(reviewsByUser.at(l).second.first, reviewsByUser.at(l).second.second));
            }
        }
    }


    unordered_map<int, vector<float>> averageReviews;

    // for all of the reviews by title, push them into an unordered map
    for (unsigned int m = 0; m < reviewsByTitle.size(); m++)
    {
        averageReviews[reviewsByTitle.at(m).first].push_back(reviewsByTitle.at(m).second);
    }

    vector<pair<float, int>> averages;

    // use map to add all ratings for each movie and divide by number of ratings to get average
    unordered_map<int, vector<float>>::iterator iter;
    for (iter = averageReviews.begin(); iter != averageReviews.end(); iter++) {
        float total = 0;
        if ((*iter).second.size() > 10) {
            for (unsigned int r = 0; r < (*iter).second.size(); r++) {
                total += (*iter).second.at(r);
            }
            averages.push_back(make_pair(((total) / (float) (*iter).second.size()), (*iter).first));
        }
    }

    // sort averages from greatest to least
    sort(averages.begin(), averages.end(), greater<>());

    // if there are less than 10 movies, print them all
    if (averages.size() < 10)
    {
        for (unsigned int p = 0; p < averages.size(); p++)
        {
            Node* tempID = searchMovieID(root, averages.at(p).second);
            if (tempID != nullptr)
            {
                float rate = round(averages.at(p).first * 10.0) / 10.0;
                cout << p + 1 << ". " << tempID->movieName << ", Rating: " << fixed << setprecision(1) << rate << endl;
            }
        }
    }

        // otherwise, print only 10 highest movies
    else
    {
        for (unsigned int p = 0; p < 10; p++)
        {
            Node* tempID = searchMovieID(root, averages.at(p).second);
            if (tempID != nullptr)
            {
                float rate = round(averages.at(p).first * 10.0) / 10.0;
                cout << p + 1 << ". " << tempID->movieName << ", Rating: " << fixed << setprecision(1) << rate << endl;
            }
        }
    }

    //Print out how long it took
    auto stop = chrono::high_resolution_clock::now();
    auto duration = (stop - start);
    auto time = chrono::duration_cast<chrono::milliseconds>(duration);

    cout << endl << "This process took " << time.count() << " milliseconds using a Red-Black Tree!" << endl << endl;
}

void Rbt::avoidBasedOnReview(Node *root, string movie)
{
    //Clock to keep track of how long it took
    auto start = chrono::high_resolution_clock::now();

    // search for movie that user input
    Node* temp = searchMovie(root, movie);
    if (temp == nullptr)
    {
        cout << "Invalid Movie! Please enter a valid movie title" << endl << endl;
        return;
    }

    vector<pair<float, int>> lowReviews;

    // for all the reviews of that movie
    for (unsigned int i = 0; i < temp->reviews.size(); i++)
    {
        // if it is less than or equal to 2.0, push into a vector
        if (temp->reviews.at(i).second <= 2.0)
        {
            lowReviews.push_back(make_pair(temp->reviews.at(i).second, temp->reviews.at(i).first));
        }
    }


    // sort the reviews from lowest to highest
    sort(lowReviews.begin(), lowReviews.end());

    vector<pair<float, int>> temporary;

    // if there are more than 100 reviews, keep only 100 lowest
    if (lowReviews.size() > 100)
    {
        for (unsigned int k = 0; k < 100; k++)
        {
            temporary.push_back(make_pair(lowReviews.at(k).first, lowReviews.at(k).second));
        }
        lowReviews.clear();
        lowReviews = temporary;
        temporary.clear();
    }

    vector<pair<int, float>> reviewsByTitle;

    // for all the low reviews
    for (unsigned int h = 0; h < lowReviews.size(); h++)
    {
        // for every review sorted by user
        for (unsigned int l = 0; l < reviewsByUser.size(); l++)
        {
            // if the userIDs are the same, the movie isn't the original movie, and the rating is less than or equal to 2.0, push into a vector
            if (reviewsByUser.at(l).first == lowReviews.at(h).second && reviewsByUser.at(l).second.first != temp->id && reviewsByUser.at(l).second.second <= 2.0)
            {
                reviewsByTitle.push_back(make_pair(reviewsByUser.at(l).second.first, reviewsByUser.at(l).second.second));
            }
        }
    }


    unordered_map<int, vector<float>> averageReviews;

    // push all reviews sorted by movie into an unordered map
    for (unsigned int m = 0; m < reviewsByTitle.size(); m++)
    {
        averageReviews[reviewsByTitle.at(m).first].push_back(reviewsByTitle.at(m).second);
    }

    vector<pair<float, int>> averages;

    // add all reviews for each movie and divide by number of reviews to get average
    unordered_map<int, vector<float>>::iterator iter;
    for (iter = averageReviews.begin(); iter != averageReviews.end(); iter++)
    {
        float total = 0;
        if ((*iter).second.size() > 10)
        {
            for (unsigned int r = 0; r < (*iter).second.size(); r++)
            {
                total += (*iter).second.at(r);
            }
            averages.push_back(make_pair(((total) / (float) (*iter).second.size()), (*iter).first));
        }
    }

    // sort reviews from lowest to highest
    sort(averages.begin(), averages.end());

    // push averages into a vector
    vector<pair<float, string>> averageMovies;
    for (unsigned int e = 0; e < averages.size(); e++)
    {
        Node* tempID = searchMovieID(root, averages.at(e).second);
        if (tempID != nullptr)
        {
            averageMovies.push_back(make_pair(averages.at(e).first, tempID->movieName));
        }
    }

    // sort movies again from lowest to highest
    sort(averageMovies.begin(), averageMovies.end());

    // if there are less than 10 movies, print them all
    if (averageMovies.size() < 10)
    {
        for (unsigned int p = 0; p < averageMovies.size(); p++)
        {
            float rate = round(averages.at(p).first * 10.0) / 10.0;
            cout << p + 1 << ". " << averageMovies.at(p).second << ", Rating: " << fixed << setprecision(1) << rate << endl;
        }
    }

        // otherwise, print only 10 lowest
    else
    {
        for (unsigned int p = 0; p < 10; p++)
        {
            float rate = round(averages.at(p).first * 10.0) / 10.0;
            cout << p + 1 << ". " << averageMovies.at(p).second << ", Rating: " << fixed << setprecision(1) << rate << endl;
        }
    }

    //Print out how long it took
    auto stop = chrono::high_resolution_clock::now();
    auto duration = (stop - start);
    auto time = chrono::duration_cast<chrono::milliseconds>(duration);

    cout << endl << "This process took " << time.count() << " milliseconds using a Red-Black Tree!" << endl << endl;
}

void Rbt::bestOverallMovies(vector<Node*>& traversePtrs)
{
    //Clock to keep track of how long it took
    auto start = chrono::high_resolution_clock::now();

    //Variables
    vector<pair<string, float>> result;

    //Iterate through ALL movies and calculate their ratings
    for (int i = 0; i < traversePtrs.size(); i++)
    {
        if (traversePtrs[i]->reviews.size() > 100)
        {
            result.emplace_back(make_pair(traversePtrs[i]->movieName, calculateAverageRatings(traversePtrs[i]->reviews)));
        }
    }

    //Sort the movies from highest to lowest rating
    sort(result.begin(), result.end(), [] (const auto &movie1, const auto &movie2) {return movie1.second > movie2.second;});

    //Print out top 10 movies
    for (int i = 0; i < 10; i++)
    {
        float rating = round(result[i].second * 10.0) / 10.0;
        cout <<  i + 1 << ". " << result[i].first << ", Rating: " << fixed << setprecision(1) << rating <<endl;
    }

    //Print out how long it took
    auto stop = chrono::high_resolution_clock::now();
    auto duration = (stop - start);
    auto time = chrono::duration_cast<chrono::milliseconds>(duration);

    cout << endl << "This process took " << time.count() << " milliseconds using a Red-Black Tree!" << endl << endl;
}

void Rbt::worstOverallMovies(vector<Node *> &traversePtrs)
{
    //Clock to keep track of how long it took
    auto start = chrono::high_resolution_clock::now();

    //Variables
    vector<pair<string, float>> result;

    //Iterate through ALL movies and calculate their ratings
    for (int i = 0; i < traversePtrs.size(); i++)
    {
        if (traversePtrs[i]->reviews.size() > 100)
        {
            result.emplace_back(make_pair(traversePtrs[i]->movieName, calculateAverageRatings(traversePtrs[i]->reviews)));
        }
    }

    //Sort from highest to lowest
    sort(result.begin(), result.end(), [] (const auto &movie1, const auto &movie2) {return movie1.second < movie2.second;});

    //Print out top 10 movies
    for (int i = 0; i < 10; i++)
    {
        float rating = round(result[i].second * 10.0) / 10.0;
        cout <<  i + 1 << ". " << result[i].first << ", Rating: " << fixed << setprecision(1) << rating <<endl;
    }

    //Print out how long it took
    auto stop = chrono::high_resolution_clock::now();
    auto duration = (stop - start);
    auto time = chrono::duration_cast<chrono::milliseconds>(duration);

    cout << endl << "This process took " << time.count() << " milliseconds using a Red-Black Tree!" << endl << endl;
}

//---------------------READING IN FILES------------------------//

vector<Node*> getNodes(Node* root,vector<Node*>& vectors)
{
    vector<Node*> movieNodes = vectors;

    if (root != nullptr)
    {
        movieNodes = getNodes(root->left, movieNodes);
        movieNodes.push_back(root);
        movieNodes = getNodes(root->right, movieNodes);
    }

    return movieNodes;
}

vector<pair<int, vector<pair<int, float>>>> getReviewsFromMovie(string fileName, vector<pair<int, pair<int, float>>>& reviewsByUser)
{
    //Variables
    vector<pair<int, vector<pair<int, float>>>> reviews;
    pair<int, vector<pair<int,float>>> userReviews;
    unordered_map<int, int> visited;

    ifstream inFile(fileName);

    if (inFile.is_open()) {
        //1.Read heading data from the file
        string lineFromFile;
        getline(inFile, lineFromFile);

        for (unsigned int i = 0; i < 27753444; i++)
        {
            getline(inFile, lineFromFile);
            istringstream stream(lineFromFile);

            //Variables
            string tempUserID;
            int userID = 0;
            string tempRating;
            float rating = 0.0;
            string tempMovieID;
            int movieID = 0;

            //Start reading in with getline
            getline(stream, tempUserID, ',');
            userID = stoi(tempUserID);

            getline(stream, tempMovieID, ',');
            movieID = stoi(tempMovieID);
            getline(stream, tempRating, ',');
            rating = stof(tempRating);

            if (visited.find(movieID) != visited.end())
            {
                reviews.at(visited[movieID]).second.emplace_back(make_pair(userID, rating));
            }
            else
            {
                vector<pair<int, float>> temp;
                temp.push_back(make_pair(userID, rating));
                reviews.push_back(make_pair(movieID, temp));
                visited[movieID] = reviews.size() - 1;
            }
            reviewsByUser.push_back(make_pair(userID, make_pair(movieID, rating)));
        }
    }
    return reviews;
}

void readIntoRbt(string fileName, Rbt& rbt)
{
    ifstream inFile(fileName);

    if (inFile.is_open()) {
        //1.Read heading data from the file
        string lineFromFile;
        getline(inFile, lineFromFile);

        //Variables
        int count = 0;
        vector<pair<int, pair<int, float>>> reviewsByUser;
        vector<pair<int, vector<pair<int, float>>>> userReviews = getReviewsFromMovie("ratings 2.csv", reviewsByUser);

        //Sort the userReviews based on movieID to be able to access it
        sort(userReviews.begin(), userReviews.end());

        for (unsigned int j = 0; j < 58098; j++) {
            getline(inFile, lineFromFile);
            istringstream stream(lineFromFile);

            //Variables
            string tempMovieName;
            string movieName;
            string tempGenres;
            vector<string> genres;
            string tempMovieID;
            int movieID = 0;

            //Start reading in with getline
            getline(stream, tempMovieID, ',');
            movieID = stoi(tempMovieID);

            //Get movie name
            getline(stream, tempMovieName);

            //Checks if the movie title starts with quotation
            if (tempMovieName.at(0) == '\"') {
                int index = 0;
                for (int i = 1; i < tempMovieName.size(); i++) {
                    if (tempMovieName.at(i) == '\"') {
                        index = i + 2;
                        break;
                    }
                    movieName += tempMovieName.at(i);
                }
                tempGenres = tempMovieName.substr(index, tempMovieName.size());
            }
                //If the movie title does not start with quotation
            else
            {
                int index = 0;
                for (int i = 0; i < tempMovieName.size(); i++)
                {
                    if (tempMovieName.at(i) == ',')
                    {
                        index = i + 1;
                        break;
                    }
                    movieName += tempMovieName.at(i);
                }
                tempGenres = tempMovieName.substr(index, tempMovieName.size());
            }

            //Fills up the Genre vector
            string tempString;
            if (tempString != "(no genres listed)")
            {
                for (int i = 0; i < tempGenres.size(); i++)
                {
                    if (tempGenres.at(i) == '|' || i == tempGenres.size() - 1)
                    {
                        genres.push_back(tempString);
                        tempString = "";
                        continue;
                    }
                    tempString += tempGenres.at(i);
                }
            }

            vector<pair<int, float>> temporaryVec;
            for (unsigned int w = 0; w < userReviews.size(); w++)
            {
                if (userReviews.at(w).first == movieID)
                {
                    temporaryVec = userReviews.at(w).second;
                    break;
                }
            }

            //Insert the node into the rbt (Red-Black Tree)
            rbt.insertNode(movieName, movieID, temporaryVec, genres);
        }
        rbt.updateReviewsByUser(reviewsByUser);
    }
}

