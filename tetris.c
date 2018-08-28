#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;//1주차 과제에서 2번째 nextblock을 초기화
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);
	
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		move(10+i,WIDTH+13);//1주차 과제에서 2번째 nextblock을 그리기 위해 기존블록창 아래 위치에 커서를 이동.
		for( j = 0; j < 4; j++ ){//2번째 nextblock을 그린다.
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j;
	for(i =0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j] == 1){
				if(f[blockY+i][blockX+j] == 1){
					return 0;
				}
				else if(blockY+i>=HEIGHT){
					return 0;
				}
				else if(blockX+j<0){
					return 0;
				}
				else if(blockX+j>=WIDTH){
					return 0;
				}
			}		
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j,k,check,yd;
	int blk= currentBlock, rot = blockRotate, y = blockY, x = blockX;
	switch(command){
		case KEY_UP:
			rot = (rot+3)%4;
			break;
		case KEY_DOWN:
			y--;
			break;
		case KEY_LEFT:
			x++;
			break;
		case KEY_RIGHT:
			x--;
			break;
	}
        for(i = y; i <22; i++){//1주차 과제에서 그려져있던 DrawShadow를 찾아 지우는 부분이다.
                check = 0;
                for(j = 3; j >= 0; j--){
                        for(k = 0; k < 4; k++){
                                if(block[blk][rot][j][k] == 1){
                                        if(i+j ==  22){
                                                check++;
                                                break;
                                        }
                                        else if(field[i+j][x+k] == 1){
                                                check++;
                                                break;
                                        }
                                }
                        }
                }
                if(check != 0){
                        yd = i-1;
                        break;
                }
        }
	for(i=0;i<4;i++){//1주차 실습에서 기존의 블록의 위치를 지우는 역할을 한다.
		for(j=0;j<4;j++){
			if(block[blk][rot][i][j] == 1){
				move(i+y+1,j+x+1);
				printw(".");
				move(i+yd+1,j+x+1);
				printw(".");
			}
		}
	}
	DrawShadow(blockY,blockX, currentBlock, blockRotate);//그림자를 먼저 그려야 마지막에 블록이 남는다.
	DrawBlock(blockY, blockX, currentBlock, blockRotate,' ');
	move(HEIGHT,WIDTH+10);
}

void BlockDown(int sig){
	int i,j,flag;
	int check;
	int prevscore=score;
	flag = CheckToMove(field, nextBlock[0],blockRotate,blockY+1,blockX);
	if(flag == 0){//블록이 내려갈 수 없는 경우
		if(blockY == -1){//블록의 위치가 라인을 벗어난 경우gameover처리를 해준다.
			gameOver = 1;
		}
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);
		if(score != prevscore){
			PrintScore(score);
			DrawField();
		}//점수 계산을 하고 다시 그려준다.
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;
		DrawNextBlock(nextBlock);
		blockRotate =0;
		blockY = -1;
		blockX = WIDTH/2-2;
	}//기존 블록의 대한 정보를 초기화 해준다.
	else{
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0],blockRotate,blockY,blockX);
	}//블록이 내려갈수 있는 경우 블록을 1칸 내린다.
	timed_out=0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j,touched = 0;
	int check;//기존의 쌓인 블록과 현재 쌓인 블록이 만나는지 판별할 때 사용된다.
	for(j = 0; j < 4; j++){
		check = 0;
		for(i = 3; i >= 0; i--){
			if(block[currentBlock][blockRotate][i][j] == 1){
				check++;
				f[blockY+i][blockX+j] = 1;
				if(blockY+i == 21){
					touched++;
				}//라인이 21인 경우 필드 라인에 붙은 경우임으로 점수를 plus해준다.
				else if(check == 1){
					if(f[blockY+i+1][blockX+j]==1){
						touched++;
					}
				}//기존의 쌓인 블록과 현재 쌓은 블록이 만나는 경우 점수를 plus해준다.
			}
		}
	}
	return (touched*10);//현재 쌓은 블록이 기존의 필드의 밑부분과 닿는 부분을 계산하여 점수를 리턴한다.
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i, j,k;
	int check,scorec=0;
	for(i = 21; i>0;i--){//필드 가장 아래의 라인에서부터 찾기 시작한다.
		check =0;		
		for(j = 0; j<10;j++){
			if(f[i][j] == 1){
				check++;
			}
		}
		if(check == 10){//필드의 한 라인이 가득 찬 경우
			scorec++;//점수를 올려준다.
			for(k =i;k>=1;k--){//해당라인 위를 전부 아래로 1줄씩 내려준다.
				for(j=0;j<10;j++){
					f[k][j] = f[k-1][j];
				}
			}
			for(j=0;j<10;j++){//맨 윗줄은 0으로 초기화 해준다.
				f[0][j] = 0;
			}
			i++;
		}
	}
	return (scorec*scorec*100);//지운 라인에 따라 점수를 리턴
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int i,j,k,check;
	int yd;
	for(i = y; i <22; i++){
		check = 0;//해당 x의 위치에서 가장 밑바닥인지를 판별할 때 사용된다.
		for(j = 3; j >= 0; j--){
			for(k = 0; k < 4; k++){
				if(block[blockID][blockRotate][j][k] == 1){
					if(i+j ==  22){//필드라인 밑바닥과 만나는 경우
						check++;
						break;
					}
					else if(field[i+j][x+k] == 1){//기존의 쌓인 필드의 블록과 만나는 경우
						check++;
						break;
					}
				}
			}
		}
		if(check != 0){//그림자가 그려질 y의 좌표를 저장한다.
			yd = i-1;
			break;
		}
	}
	DrawBlock(yd,x,blockID,blockRotate,'/');//그림자를 그린다.
}

void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
