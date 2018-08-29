#include "tetris.h"

static struct sigaction act, oact;

Node *head=NULL, *currnode=NULL;
int Nodenum=0;
int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	
	
	srand((unsigned int)time(NULL));
	createRankList();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
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
	
//	DrawRecommend(recommendY, recommendX, currentBlock, recommendR);
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
//		for(i=0;i<HEIGHT;i++){
//			for(j=0;j<WIDTH;j++){
//				recRoot->recField[i][j] = field[i][j];
//			}
//		}
//		recRoot->level = 3;
//		recommend(recRoot);
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
	FILE *fp;
	int i;
	char user[21];
	int scorec;
	Node *temp;
	fp = fopen("rank.txt","r");
	fscanf(fp,"%d",&Nodenum);
	if(Nodenum != 0){
		for(i = 0; i<Nodenum; i++){
			temp = (Node*)malloc(sizeof(Node));
			fscanf(fp,"%s %d\n", user, &scorec);
			strcpy(temp->name,user);
			temp->score = scorec;
			temp->next = NULL;
			if(head == NULL){
				head = temp;
				currnode = temp;
			}
			else{
				currnode->next = temp;
				currnode = temp;
			}
		}
	}
	fclose(fp);
}

void rank(){
	// user code
	clear();
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	switch(wgetch(stdscr)){
		case '1': rankfirst(); break;
		case '2': ranksecond(); break;
		case '3': rankthird(); break; 
	}
}
void rankfirst(){//menu의 rank시스템 중 첫번째 기능
	int x=0,y=0;
	int i=1;
	Node *target;
	target = head;
	echo();
	printw("X: ");
	scanw("%d",&x);
	printw("Y: ");
	scanw("%d", &y);
	printw("	name	|	score\n");
	printw("--------------------------------\n");
	if(Nodenum != 0){//파일이 비어있지 않을 경우 실행.
		if(x != 0 && y != 0){//x와 y를 모두 받아들였을 경우 실행.
			if(x<=y){
				while(i<=y){
					if(x<=i){
						printw("%s		| %d\n",target->name,target->score);
					}
					target = target->next;
					i++;
				}	
			}
			else {
				printw("search failure: no rank in the list\n");
			}
		}
		else if(x == 0 && y != 0){//y의 값만 받아들였을 경우 실행.
			while(i<=y){
				printw("%s		| %d\n",target->name,target->score);
				target = target->next;
				i++;
			}
		}
		else if(x != 0 && y == 0){//x의 값만 받아들였을 경우 실행.
			while(i <= Nodenum){
				if(i>=x){
					printw("%s		| %d\n",target->name,target->score);
				}
				target = target->next;
				i++;
			}
		}
		else if(x == 0 && y == 0){//x와 y 둘다 받지 않았을 경우 실행.
				while(1){
				printw("%s		| %d\n",target->name,target->score);
				if(target->next == NULL){
						break;
				}
				target = target->next;
				i++;
			}
		}
	}
	else{//파일이 비어있을 경우 실행.
		printw("\n	empty!!\n\n");
	}
	scanw("%d",&x);
	noecho();
}
void ranksecond(){//메뉴에서 rank시스템 중 2번째 기증을 구현한 함수.
	char user[NAMELEN];
	int i=0;
	Node *target;
	target = head;
	echo();
	printw("Input the name: ");
	scanw("%s",user);
	printw("	name	|	score\n");
	printw("--------------------------------\n");
	while(1){
		if(strcmp(target->name,user)==0){//찾고자 하는 정보와 일치하면 출력.
			printw("%s		| %d\n",target->name,target->score);
			i++;
		}
		if(target->next == NULL){//모든 리스트를 찾아보고 실행된다.
			if(i == 0){//찾고자 하는 정보가 없을 경우 출력.
				printw("\nsearch failure: no name in the list\n");
			}
			break;
		}
		target = target->next;
	}
	scanw("%d",&i);
	noecho();
}
void rankthird(){
	int i,ranknum;
	Node *target;
	Node *rmnode;
	target = head;
	echo();
	printw("Input the rank: ");
	scanw("%d",&ranknum);
	if(ranknum > Nodenum){//랭킹을 벗어나는 수를 받았을 겨우 실행.
		printw("\nSearch failure: the rank not in the list\n");
	}
	else{
		if(ranknum == 1){//링크드리스트의 첫부분을 지울경우 실행.
			rmnode = target;
			head = target->next;
		}
		else{
			for(i = 1; i < ranknum-1; i++){
				target = target->next;
			}
			rmnode = target->next;
			target->next = target->next->next;
		}
		printw("\nresult: the rank deleted\n");
		free(rmnode);
		Nodenum--;
		writeRankFile();
	}
	scanw("%d",&ranknum);
	noecho();
}
void writeRankFile(){//rank.txt에 처리한 정보를 저장한다.
	// user code
	FILE *fp;
	int i=0;
	Node *target;
	target = head;
	fp = fopen("rank.txt","w");
//	if(Nodenum != 0){
		fprintf(fp,"%d\n", Nodenum);
		for(i=0; i<Nodenum; i++){
			fprintf(fp,"%s %d\n",target->name,target->score);
			target = target->next;
		}
//	}
	fclose(fp);
}

void newRank(int score){//게임이 끝나고 새로운 정보를 받아들여 랭크에 추가한다.
	// user code
	char user[NAMELEN];
	Node *target;
	Node *new;
	new = (Node*)malloc(sizeof(Node));	
	clear();
	echo();
	printw("your name: ");
	scanw("%s",user);
	strcpy(new->name,user);
	new->score = score;
	new->next = NULL;
	target = head;
	while(1){
		if(Nodenum == 0){//비어있는 링크드리스트일 경우.
			head = new;
			currnode = new;
			break;
		}
		else if(target->next == NULL){//링크드리스트의 끝에 붙일 겨우.
			target->next = new;
			break;
		}
		else if(target->score > score && score >= target->next->score){
			new->next = target->next;
			target->next = new;
			break;
		}
		else{
			target = target->next;
		}
	}
	Nodenum++;
	writeRankFile();//파일에 저장한다.
	noecho();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	DrawBlock(y,x,blockID,blockRotate,'R');
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int i,j,k,n=0,num=0,o,p;

	char tempfield[HEIGHT][WIDTH];
	typedef struct _com{
		int score;	
		int x;
		int y;
		int r;
	} Com;
	Com *comp;
	comp = (Com*)malloc(sizeof(Com)*34);
	for(i=0;i<HEIGHT;i++){
		for(j =0;j<WIDTH;j++){
			tempfield[i][j] = root->recField[i][j];
		}
	}
	for(i=0;i<4;i++){
		for(j =-1;j<9;j++){
			for(k=21;k>=0;k--){
				if(CheckToMove(tempfield, nextBlock[root->level], i, k, j) == 1){
					root->c[n] = (RecNode*)malloc(sizeof(RecNode));
					root->c[n]->acScore = AddBlockToField(tempfield, nextBlock[root->level], i, k, j);//블록을 임의의 필드에 저장.
					for(o=0;o<HEIGHT;o++){
						for(p=0;p<WIDTH;p++){
							root->c[n]->recField[o][p] = tempfield[o][p];
						}
					}
					root->c[n]->level = root->level+1;
					if(root->c[n]->level != recRoot->level){
						root->c[n]->acScore += recommend(root->c[n]);
					}
					comp[n].score = root->c[n]->acScore;
					comp[n].x = j;
					comp[n].y = k;
					comp[n].r = i;
					n++;
				}
			}
		}
	}
	for(i=1; i<n; i++){
		if(comp[i].score >0){
			if(comp[num].score < comp[i].score){
				num = i;
			}
		}
	}
	recommendR = comp[num].r;
	recommendX = comp[num].x;
	recommendY = comp[num].y;
	max = comp[num].score;
	return max;
}

void recommendedPlay(){
	// user code
}
