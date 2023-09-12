#define _USE_MATH_DEFINES

#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

class Glicko {
    private: 
        double _RD;
        double _unratedRD;
        double _c;
        double _rating;
        double _d2;
    public:
        /*
        avgRD: Rating deviation of average player

        unratedRD: Rating deviation of an unrated player

        timeToUnrated: how much time it takes to get back to Rating deviation of an unrated player
        */
        Glicko(double avgRD, double unratedRD, double timeToUnrated, double initRating) 
        : _unratedRD(unratedRD), _rating(initRating) 
        {
            _c = calcCofRD(avgRD, _unratedRD, timeToUnrated);
        }

        /*
        calcCofRD: calculates the c constant of the rating deviation formula

        Parameters

        RD: rating deviation

        RDo: intial rating deviation

        t: time since last competition/match

        unratedRD: RD of an unrated player

        return value

        c: constant for uncertainty of a player's skill over a certain amount of time (can be calculated by solving for c for the amount of time that would pass before RD returns to that of an unrated player)
        */
        double calcCofRD(double RD, double RDo, double t) {
            return sqrt(((RDo * RDo) - (RD * RD))/t);
        }

        /*
    calcRD: calculates the rating deviation for a player

    Parameters

    RD: rating deviation

    c: constant for uncertainty of a player's skill over a certain amount of time (can be calculated by solving for c for the amount of time that would pass before RD returns to that of an unrated player)

    t: time since last competition/match

    unratedRD: RD of an unrated player

    return value

    RD: rating deviation    
*/
        double calcRD(double RD, double c, double t, double unratedRD) {
            _RD = min(sqrt((RD * RD) + c*c*t), unratedRD);
            return _RD;
        }

        double calcRD(double t) {
            _RD = min(sqrt((_RD * _RD) + _c*_c*t), _unratedRD);
            return _RD;
        }

        double RD() { return _RD; }

        double c() { return _c; }

        double rating() { return _rating; }

        /* 
            newRating: calculate the new rating of the player

            Parameters

            s: win, loss, or draw (1, 1/2, 0)

            ro: initial rating of the player

            RD: rating deviation of the player

            ropp: rating of the opponent or average rating of the opponents

            RDopp: rating deviation of the opponent or average rating deviation of the opponents
        */
        double newRating (double s, double ro, double RD, double ropp, double RDopp) {
            double q, g_out, E_out;

            q = log(10) / 400;
            cout << "Q Score: " << q << endl;
            g_out = g(RDopp, q);
            E_out = E(g_out, ro, ropp);
            _d2 = d2(q, g_out, E_out);

            _rating = ro + q / (1 / (RD * RD) + 1 / (_d2)) * g_out * (s - E_out);
            return _rating;
        }
        /* 
            newRating: calculate the new rating of the player

            Parameters

            s: win, loss, or draw (1, 1/2, 0)

            ropp: rating of the opponent or average rating of the opponents

            RDopp: rating deviation of the opponent or average rating deviation of the opponents
        */
        double newRating (double s, double ropp, double RDopp) {
            double q, g_out, E_out;

            // q = log(10) / 400;
            // cout << "Q Score: " << q << endl;
            q = 25;
            g_out = g(RDopp, q);
            E_out = E(g_out, _rating, ropp);
            _d2 = d2(q, g_out, E_out);

            _rating = _rating + q / (1 / (_RD * _RD) + 1 / (_d2)) * g_out * (s - E_out);
            _rating > 1 ? _rating : _rating = 1;
            return _rating;
        }

        double newRD (double RD, double d2) {
            double RDprime;
            RDprime = sqrt(1/(1/(RD*RD) + 1/d2));
            return RDprime;
        }

        double newRD () {
            _RD = sqrt(1/(1/(_RD*_RD) + 1/_d2));
            return _RD;
        }

        double g(double RDopp, double q) {
            return 1 / sqrt(1 + (3 * (q * q) * (RDopp * RDopp)) / (M_PI * M_PI));
        }

        double E(double g, double ro, double ropp) {
            return 1 / (1 + pow(10,(g * (ro - ropp))/-400));
        }

        double d2(double q, double g, double E) {
            return 1 / ((q * q) * (g * g) * E * (1 - E));
        }
};




int main (void) {
    double aRD, uRD, TTU, TSC, IR;
    cout << "avgRD: ";
    cin >> aRD;
    cout << "unratedRD: ";
    cin >> uRD;
    cout << "time to unrated: ";
    cin >> TTU;
    cout << "initial rating: ";
    cin >> IR;

    Glicko * rating = new Glicko(aRD, uRD, TTU, 2000);
    cout << "time since last competition: ";
    cin >> TSC;
    rating->calcRD(TSC);
    cout << "c: " << rating->c() << "\nnew RD: " << rating->RD() << endl;
    int playing = 1, result;
    while (playing) {
        cout << "Win or Lose?";
        cin >> result;
        rating->newRating(result, 2000, 100);
        rating->newRD();
        cout << "\nnewRating: " << rating->rating() << "\nnew RD: " << rating->RD() << endl;
        cout << "Playing? ";
        cin >> playing;
    }

    return 0;
}

