#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool is_a_winner[MAX];
void reset_is_a_winner(void);
bool check_cycle(int winner, int loser);
int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }
    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = candidate_count - 2; i >= 0; i--)
    {
        for (int j = candidate_count - 1; j > i; j--)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Selection sort
    int t_w, t_l, min;
    for (int i = 0; i < pair_count - 2; i++)
    {
        min = i;
        for (int j = i + 1; j < pair_count - 1; j++)
        {
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[j].winner][pairs[j].loser])
            {
                min = j;
            }
        }
        t_w = pairs[min].winner;
        t_l = pairs[min].loser;
        // After you find the smallest pair of the array you exchange it with the ith
        pairs[min].winner = pairs[i].winner;
        pairs[min].loser = pairs[i].loser;
        pairs[i].winner = t_w;
        pairs[i].loser = t_l;
    }

    return;
}

void reset_is_a_winner(void)
{
    for (int k = 0; k < candidate_count; k++)
    {
        is_a_winner[k] = false;
    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        // Set all the candidates counts' flags on False (if they are on
        // true it means they're already a winner and hence we have a cycle)
        reset_is_a_winner();
        // We start locking the couple so we can flag it and check if there is a cycle
        locked[pairs[i].winner][pairs[i].loser] = true;

        if (!check_cycle(pairs[i].winner, pairs[i].loser) == false) //true
        {
            // This means there is a cycle so we "unlock" this pairs
            locked[pairs[i].winner][pairs[i].loser] = false; // false
        }
        // PRINTING SECTION FOR LOCK_PAIRS
        //printf("locked[%i][%i] = %s\n", pairs[i].winner,pairs[i].loser, locked[pairs[i].winner][pairs[i].loser] ? "true" : "false");
    }
    return;
}

bool check_cycle(int winner, int loser)
{
    //Check if the winner has already been flagged on a previous cycle, if so that means there is a cycle
    if (is_a_winner[winner])
    {
        return true;
    }
    // This means this is the first time the winner is checked and so we flag it on true
    is_a_winner[winner] = true;

    // Check if the loser is the winner in any of the couples already locked
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[loser][i])
        {
            // If we already locked this couple and the winner has been already flagged, it means we have a cycle.
            if (is_a_winner[i])
            {
                return true;
            }
            else
            {
                // If we haven't already flagged the winner this means it's the first time we check it, so we keep on checking the locked couples
                if (check_cycle(i, winner))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    bool winner[MAX];

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j])
            {
                winner[i] = true;
            }
            for (int k = 0; k < candidate_count; k++)
            {
                if (locked[k][i])
                {
                    winner[i] = false;
                }
            }
        }
        if (winner[i])
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}
