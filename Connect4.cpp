#include<bits/stdc++.h>
using namespace std;

char board[6][7];
string line;

class Position
{
    uint64_t current_position,mask;
    int moves;

    static bool alignment(uint64_t pos) //checks if 4 coins are connected
    {
        // horizontal 
        uint64_t m=pos&(pos>>(HEIGHT+1));
        if(m&(m>>(2*(HEIGHT+1)))) return true;

        // diagonal 1
        m=pos&(pos>>HEIGHT);
        if(m&(m>>(2*HEIGHT))) return true;

        // diagonal 2 
        m=pos&(pos>>(HEIGHT+2));
        if(m&(m>>(2*(HEIGHT+2)))) return true;

        // vertical;
        m=pos&(pos>>1);
        if(m&(m>>2)) return true;

        return false;
    }

    static uint64_t top_mask(int col) //returns mask with top row = 1
    {
        return (UINT64_C(1)<<(HEIGHT-1))<<col*(HEIGHT+1);
    }

    static uint64_t bottom_mask(int col) //returns mask with bottom row = 1
    {
        return UINT64_C(1)<<col*(HEIGHT+1);
    }

    static uint64_t column_mask(int col) //returns mask with particular column = 1
    {
        return ((UINT64_C(1)<<HEIGHT)-1)<<col*(HEIGHT+1);
    }
    public:

    static const int WIDTH=7,HEIGHT=6,MIN_SCORE=-(WIDTH*HEIGHT)/2+3,MAX_SCORE=(WIDTH*HEIGHT+1)/2-3;

    bool canPlay(int col) const //returns if column has space
    {
        return (mask&top_mask(col))==0;
    }

    void play(int col) //drops coin into column
    {
        current_position^=mask;
        mask|=mask+bottom_mask(col);
        moves++;
    }

    unsigned int play(std::string seq) //loads position
    {
        for(unsigned int i = 0; i < seq.size(); i++) 
        {
            int col;
            col=seq[i]-'1';
            play(col);
        }
        return seq.size();
    }

    bool isWinningMove(int col) const //returns if dropping coin into column wins game
    {
        uint64_t pos=current_position; 
        pos|=(mask+bottom_mask(col))&column_mask(col);
        return alignment(pos);
    }

    int nbMoves() const //returns number of moves played so far
    {
        return moves;
    }

    uint64_t key() const //returns current position of board
    {
        return current_position + mask;
    }

    Position() //constructor
    {
        current_position=0;
        mask=0;
        moves=0;
    }
};

class HashTable
{
    struct Entry
    {
        uint64_t key: 56; 
        uint8_t val;      
    };                  

    vector<Entry> T;

    int index(uint64_t key) const //returns index value of key in vector
    {
        return key%T.size();
    }

    public:

    HashTable(int size): T(size) //constructor
    {
        assert(size > 0);
    }

    void reset() //empties vector
    {
        memset(&T[0], 0, T.size()*sizeof(Entry));
    }

    void put(uint64_t key, uint8_t val) //enters into vector
    {
        int i = index(key); 
        T[i].key = key;              
        T[i].val = val;       
    }

    uint8_t get(uint64_t key) const //returns score of position
    {
        int i = index(key);  
        if(T[i].key == key) 
            return T[i].val;            
        else 
            return 0;                   
    }
};

class Solver 
{
    int columnOrder[Position::WIDTH];
    HashTable Table;

    int minmax(const Position &P, int alpha, int beta) //traversing min-max game tree using alpha-beta pruning
    {
        int x,max,val,score;
        if(P.nbMoves()==Position::WIDTH*Position::HEIGHT) 
            return 0; 
        for(x=0;x<Position::WIDTH;x++)
            if(P.canPlay(x)&&P.isWinningMove(x)) 
                return (Position::WIDTH*Position::HEIGHT+1-P.nbMoves())/2;
        max=(Position::WIDTH*Position::HEIGHT-1-P.nbMoves())/2;
        if(val=Table.get(P.key()))
            max=val+Position::MIN_SCORE-1;
        if(beta>max) 
        {
            beta=max;                     
            if(alpha>=beta) return beta;  
        }
        for(x=0;x<Position::WIDTH;x++)
            if(P.canPlay(columnOrder[x])) 
            {
                Position P2(P);
                P2.play(columnOrder[x]);               
                score=-minmax(P2,-beta,-alpha);
                if(score>=beta) return score;  
                if(score>alpha) alpha=score; 
            }
        Table.put(P.key(),alpha-Position::MIN_SCORE+1);
        return alpha;
    }

    public:

    int solve(const Position &P) //returns score of a position
    {
        return minmax(P,-Position::WIDTH*Position::HEIGHT/2,Position::WIDTH*Position::HEIGHT/2);
    }

    void reset() //empties hash table
    {
        Table.reset();
    }

    Solver() : Table(8388593) //constructor
    {
        reset();
        for(int i = 0; i < Position::WIDTH; i++)
            columnOrder[i] = Position::WIDTH/2 + (1-2*(i%2))*(i+1)/2;
    }
};

void printboardstate() //prints current board state
{
    int i,j;
    cout<<"Current board state:\n";
    for(i=0;i<6;i++)
    {
        for(j=0;j<7;j++)
            if(board[i][j]=='O') cout<<"  ";
            else cout<<board[i][j]<<' ';
        cout<<'\n';
    }
}

bool checkconnect(int col)
{
    int row,i,count=1;
    char check;
    for(row=0;row<6;row++)
        if(board[row][col]!='O')
        {
            check=board[row][col];
            break;
        }
    //horizontal
    for(i=1;col-i>-1&&board[row][col-i]==check;i++)
        count++;
    for(i=1;col+i<7&&board[row][col+i]==check;i++)
        count++;
    if(count>3) return 1;
    count=1;
    //diagonal 1
    for(i=1;row-i>-1&&col-i>-1&&board[row-i][col-i]==check;i++)
        count++;
    for(i=1;row+i<6&&col+i<7&&board[row+i][col+i]==check;i++)
        count++;
    if(count>3) return 1;
    count=1;
    //diagonal 2
    for(i=1;row-i>-1&&col+i<7&&board[row-i][col+i]==check;i++)
        count++;
    for(i=1;row+i<6&&col-i>-1&&board[row+i][col-i]==check;i++)
        count++;
    if(count>3) return 1;
    count=1;
    //vertical
    for(i=1;row-i>-1&&board[row-i][col]==check;i++)
        count++;
    for(i=1;row+i<6&&board[row+i][col]==check;i++)
        count++;
    if(count>3) return 1;
    return 0;
}

void getinput() //gets input from user
{
    int col,i;
    char a[2];
    a[1]='\0';
    cout<<"Enter column number 1-7 to drop your coin: ";
    cin>>col;
    if(col<1||col>7)
    {
        cout<<"invalid column, try again\n";
        getinput();
    }
    col--;
    for(i=5;i>=0;i--)
        if(board[i][col]=='O')
        {
            board[i][col]='R';
            a[0]=col+'1';
            line.append(a);
            if(checkconnect(col))
            {
                cout<<"congratulations\n";
                exit(0);
            }
            break;
        }
    if(i==-1)
    {
        cout<<"column full, try again\n";
        getinput();
    }
}

int main()
{
    int i,j,count,score,m,c,k;
    char a[2];
    a[1]='\0';
    string temp;
    Solver solver;
    line="2416615552";
    for(i=0;i<6;i++)
        for(j=0;j<7;j++)
            board[i][j]='O';
    for(i=0;line[i]!='\0';i+=2)
    {
        c=line[i]-'1';
        for(j=5;j>=0;j--)
            if(board[j][c]=='O')
            {
                board[j][c]='R';
                break;
            }
        c=line[i+1]-'1';
        for(j=5;j>=0;j--)
            if(board[j][c]=='O')
            {
                board[j][c]='Y';
                break;
            }
    }    
    printboardstate();
    for(i=10;i<42;i+=2)
    {
        getinput();
        m=INT_MAX;
        c=-1;
        for(j=1;j<8;j++)
        {
            if(board[0][j-1]!='O') continue;
            for(k=5;k>=0;k--)
                if(board[k][j-1]=='O')
                {
                    board[k][j-1]='Y';
                    if(checkconnect(j-1))
                    {
                        cout<<"computer plays in column "<<j<<'\n';
                        printboardstate();
                        cout<<"better luck next time\n";
                        exit(0);
                    }
                    board[k][j-1]='O';
                    break;
                }
            Position Q;
            a[0]=j+'0';
            temp=line;
            temp.append(a);
            Q.play(temp);
            solver.reset();
            score=solver.solve(Q);
            // cout<<a<<' '<<temp<<' '<<score<<'\n';
            if(score<m)
            {
                m=score;
                c=j;
            }
        }
        cout<<"computer plays in column "<<c<<'\n';
        c--;
        for(j=5;j>=0;j--)
            if(board[j][c]=='O')
            {
                board[j][c]='Y';
                a[0]=c+'1';
                line.append(a);
                break;
            }
        printboardstate();
    }
    if(i==42) cout<<"draw\n";
    return 0;
}