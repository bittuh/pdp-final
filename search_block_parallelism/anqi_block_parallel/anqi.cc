/*****************************************************************************\
 * Theory of Computer Games: Fall 2012
 * Chinese Dark Chess Library by You-cheng Syu
 *
 * This file may not be used out of the class unless asking
 * for permission first.
\*****************************************************************************/
#include<cassert>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<windows.h>
#include"anqi.hh"

static const char *tbl="KGMRNCPkgmrncpX-";

static const char *nam[16]={
	"帥","仕","相","硨","傌","炮","兵",
	"將","士","象","車","馬","砲","卒",
	"Ｏ","　"
};

static const POS ADJ[32][4]={
	{ 1,-1,-1, 4},{ 2,-1, 0, 5},{ 3,-1, 1, 6},{-1,-1, 2, 7},
	{ 5, 0,-1, 8},{ 6, 1, 4, 9},{ 7, 2, 5,10},{-1, 3, 6,11},
	{ 9, 4,-1,12},{10, 5, 8,13},{11, 6, 9,14},{-1, 7,10,15},
	{13, 8,-1,16},{14, 9,12,17},{15,10,13,18},{-1,11,14,19},
	{17,12,-1,20},{18,13,16,21},{19,14,17,22},{-1,15,18,23},
	{21,16,-1,24},{22,17,20,25},{23,18,21,26},{-1,19,22,27},
	{25,20,-1,28},{26,21,24,29},{27,22,25,30},{-1,23,26,31},
	{29,24,-1,-1},{30,25,28,-1},{31,26,29,-1},{-1,27,30,-1}
};

CLR GetColor(FIN f) {
	return f<FIN_X?f/7:-1;
}

LVL GetLevel(FIN f) {
	assert(f<FIN_X);
	return LVL(f%7);
}

// can fa eat fb?
bool ChkEats(FIN fa,FIN fb) {
	if(fa>=FIN_X)return false;
	if(fb==FIN_X)return false;
	if(fb==FIN_E)return true ;
	if(GetColor(fb)==GetColor(fa))return false;

	const LVL la=GetLevel(fa);
	if(la==LVL_C)return true ;

	const LVL lb=GetLevel(fb);
	if(la==LVL_K)return lb!=LVL_P;
	if(la==LVL_P)return lb==LVL_P||lb==LVL_K;

	return la<=lb;
}

static void Output(FILE *fp,POS p) {
	fprintf(fp,"%c%d\n",'a'+p%4,8-(p/4));
}

void Output(MOV m) {
	FILE *fp=fopen("move.txt","w");
	assert(fp!=NULL);
	if(m.ed!=m.st) {
		fputs("0\n",fp);
		Output(fp,m.st);
		Output(fp,m.ed);
		fputs("0\n",fp);
	} else {
		fputs("1\n",fp);
		Output(fp,m.st);
		fputs("0\n",fp);
		fputs("0\n",fp);
	}
	fclose(fp);
}

void BOARD::NewGame() {
	static const int tbl[]={1,2,2,2,2,2,5};
	who=-1;
	for(POS p=0;p<32;p++)fin[p]=FIN_X;
	for(int i=0;i<14;i++)cnt[i]=tbl[GetLevel(FIN(i))];
}

static FIN find(char c) {
	return FIN(strchr(tbl,c)-tbl);
}

static POS LoadGameConv(const char *s) {
	return (8-(s[1]-'0'))*4+(s[0]-'a');
}

static void LoadGameReplay(BOARD &brd,const char *cmd) {
	if(cmd[2]!='-')brd.Flip(LoadGameConv(cmd),find(cmd[3]));
	else brd.Move(MOV(LoadGameConv(cmd),LoadGameConv(cmd+3)));
}

static POS mkpos(int x,int y) {
	return x*4+y;
}

int BOARD::LoadGame(const char *fn) {
    noFight = 0;    //reset nofight counter
	FILE *fp=fopen(fn,"r");
	assert(fp!=NULL);

	while(fgetc(fp)!='\n');

	while(fgetc(fp)!='\n');

	fscanf(fp," %*c");
	for(int i=0;i<14;i++)fscanf(fp,"%d",cnt+i);

	for(int i=0;i<8;i++) {
		fscanf(fp," %*c");
		for(int j=0;j<4;j++) {
			char c;
			fscanf(fp," %c",&c);
			fin[mkpos(i,j)]=find(c);
		}
	}

	int r;
	fscanf(fp," %*c%*s%d" ,&r);
	r--;
	who=(r==0||r==1?r:-1);
	fscanf(fp," %*c%*s%d ",&r);

	for(char buf[64];fgets(buf,sizeof(buf),fp);) {
		if(buf[2]<'0'||buf[2]>'9')break;
		char xxx[16],yyy[16];
		const int n=sscanf(buf+2,"%*s%s%s",xxx,yyy);
		if(n>=1)LoadGameReplay(*this,xxx);
		if(n>=2)LoadGameReplay(*this,yyy);
	}

	fclose(fp);
	return r;
}

void BOARD::Display() const {
	HANDLE hErr=GetStdHandle(STD_ERROR_HANDLE);
	for(int i=0;i<8;i++) {
		SetConsoleTextAttribute(hErr,8);
		for(int j=0;j<4;j++)fprintf(stderr,"[%02d]",mkpos(i,j));
		if(i==2) {
			SetConsoleTextAttribute(hErr,12);
			fputs("  ",stderr);
			for(int j=0;j<7;j++)for(int k=0;k<cnt[j];k++)fputs(nam[j],stderr);
		}
		fputc('\n',stderr);
		for(int j=0;j<4;j++) {
			const FIN f=fin[mkpos(i,j)];
			const CLR c=GetColor(f);
			SetConsoleTextAttribute(hErr,(c!=-1?12-c*2:7));
			fprintf(stderr," %s ",nam[fin[mkpos(i,j)]]);
		}
		if(i==0) {
			SetConsoleTextAttribute(hErr,7);
			fputs("  輪到 ",stderr);
			if(who==0) {
				SetConsoleTextAttribute(hErr,12);
				fputs("紅方",stderr);
			} else if(who==1) {
				SetConsoleTextAttribute(hErr,10);
				fputs("黑方",stderr);
			} else {
				fputs("？？",stderr);
			}
		} else if(i==1) {
			SetConsoleTextAttribute(hErr,7);
			fputs("  尚未翻出：",stderr);
		} else if(i==2) {
			SetConsoleTextAttribute(hErr,10);
			fputs("  ",stderr);
			for(int j=7;j<14;j++)for(int k=0;k<cnt[j];k++)fputs(nam[j],stderr);
		}
		fputc('\n',stderr);
	}
	SetConsoleTextAttribute(hErr,7);
}

int BOARD::MoveGen(MOVLST &lst) const {
	if(who==-1)return false;
	lst.num=0;
	for(POS p=0;p<32;p++) {
		const FIN pf=fin[p];
		if(GetColor(pf)!=who)continue;  //not my pieces
		const LVL pl=GetLevel(pf);
		for(int z=0;z<4;z++) {
			const POS q=ADJ[p][z];
			if(q==-1)continue;
			const FIN qf=fin[q];
			if(pl!=LVL_C){if(!ChkEats(pf,qf))continue;}
			else if(qf!=FIN_E)continue;
			lst.mov[lst.num++]=MOV(p,q);
		}
		if(pl!=LVL_C)continue;
		for(int z=0;z<4;z++) {
			int c=0;
			for(POS q=p;(q=ADJ[q][z])!=-1;) {
				const FIN qf=fin[q];
				if(qf==FIN_E||++c!=2)continue;
				if(qf!=FIN_X&&GetColor(qf)!=who)lst.mov[lst.num++]=MOV(p,q);
				break;
			}
		}

	}
	return lst.num;
}
int bittuhAllMoveGen(BOARD brd, MOVLST &lst){
    if(brd.who==-1)return false;
	lst.num=0;
	for(POS p=0;p<32;p++) {
		const FIN pf=brd.fin[p];
		if(GetColor(pf)!=brd.who)continue;  //not my pieces
		const LVL pl=GetLevel(pf);
		for(int z=0;z<4;z++) {
			const POS q=ADJ[p][z];
			if(q==-1)continue;
			const FIN qf=brd.fin[q];
			if(pl!=LVL_C){if(!ChkEats(pf,qf))continue;}
			else if(qf!=FIN_E)continue;
			lst.mov[lst.num++]=MOV(p,q);
		}
		if(pl!=LVL_C)continue;
		for(int z=0;z<4;z++) {
			int c=0;
			for(POS q=p;(q=ADJ[q][z])!=-1;) {
				const FIN qf=brd.fin[q];
				if(qf==FIN_E||++c!=2)continue;
				if(qf!=FIN_X&&GetColor(qf)!=brd.who)lst.mov[lst.num++]=MOV(p,q);
				break;
			}
		}

	}
	for(POS p=0;p<32;p++){
        const FIN pf=brd.fin[p];
        if(pf == FIN_X) lst.mov[lst.num++]=MOV(p,p);

	}
	return lst.num;
}
int bittuhEatGen(BOARD brd, MOVLST &lst){
    if(brd.who==-1)return false;
	lst.num=0;
	for(POS st=0;st<32;st++) {
        const FIN stFin=brd.fin[st];
		if(GetColor(stFin)!=brd.who)continue;  //not my pieces
        const LVL stLevel=GetLevel(stFin);
        if(stLevel != LVL_C){
            for(int z=0;z<4;z++) {          //neighbor
                const POS ed=ADJ[st][z];
                if(ed==-1)continue;
                const FIN edFin=brd.fin[ed];
                if(ChkEats(stFin,edFin) && edFin!=FIN_E)
                    lst.mov[lst.num++]=MOV(st,ed);
            }
        }
        else{
            for(int z=0;z<4;z++) {
                int jump=0;
                for(POS ed=ADJ[st][z]; ed != -1 && jump < 2; ed = ADJ[ed][z]) {
                    const FIN edFin=brd.fin[ed];
                    if(edFin != FIN_E) jump++;
                    //destination is not Empty or Sealed, and is opponent
                    if(edFin < FIN_X && GetColor(edFin) != brd.who)lst.mov[lst.num++]=MOV(st,ed);
                    break;
                }
            }
        }
	}
}
int bittuhMoveGen(BOARD brd, MOVLST &lst){
    if(brd.who==-1)return false;
	lst.num=0;
	for(POS st=0;st<32;st++){
        const FIN stFin=brd.fin[st];
		if(GetColor(stFin)!=brd.who)continue;  //not my pieces
		for(int z=0;z<4;z++) {          //neighbor
            const POS ed=ADJ[st][z];
            if(ed==-1)continue;
            if(brd.fin[ed] == FIN_E)
                lst.mov[lst.num++]=MOV(st,ed);
        }
	}
}

int bittuhFlipGen(BOARD brd, MOVLST &lst){
    if(brd.who==-1)return false;
	lst.num=0;
	for(POS p=0;p<32;p++){
        const FIN pf=brd.fin[p];
        if(pf == FIN_X) lst.mov[lst.num++]=MOV(p,p);

	}

}

int bittuhNotEatGen(BOARD brd, MOVLST &lst){
    if(brd.who==-1)return false;
	lst.num=0;
	for(POS st=0;st<32;st++){
        const FIN stFin=brd.fin[st];
		if(GetColor(stFin)!=brd.who)continue;  //not my pieces
		for(int z=0;z<4;z++) {          //neighbor
            const POS ed=ADJ[st][z];
            if(ed==-1)continue;
            if(brd.fin[ed] == FIN_E)
                lst.mov[lst.num++]=MOV(st,ed);
        }
	}
	for(POS p=0;p<32;p++){
        const FIN pf=brd.fin[p];
        if(pf == FIN_X) lst.mov[lst.num++]=MOV(p,p);

	}
}

bool BOARD::ChkLose() const {
	if(who==-1)return false;

	bool existDark=false;
	//if any pieces still unrevealed?
	//is that mine?
	for(int i=0;i<14;i++) {
		if(cnt[i]==0)continue;
		if(GetColor(FIN(i))==who)return false;
		existDark=true;
	}

	bool fLive=false;
	for(POS p=0;p<32;p++)if(GetColor(fin[p])==who){fLive=true;break;}
	if(!fLive)return true;

	MOVLST lst;
	return !existDark&&MoveGen(lst)==0;
}

bool BOARD::ChkValid(MOV m) const {
	if(m.ed!=m.st) {
		MOVLST lst;
		MoveGen(lst);
		for(int i=0;i<lst.num;i++)if(m==lst.mov[i])return true;
	} else {
		if(m.st<0||m.st>=32)return false;
		if(fin[m.st]!=FIN_X)return false;
		for(int i=0;i<14;i++)if(cnt[i]!=0)return true;
	}
	return false;
}

void BOARD::Flip(POS p,FIN f) {
	if(f==FIN_X) {
		int i,sum=0;
		for(i=0;i<14;i++)    sum+=cnt[i];
		sum=rand()%sum;
		for(i=0;i<14;i++)if((sum-=cnt[i])<0)break;
		f=FIN(i);
	}
	fin[p]=f;
	cnt[f]--;
	if(who==-1)who=GetColor(f);
	who^=1;
}

void BOARD::Move(MOV m) {
	if(m.ed!=m.st) {
        if(fin[m.ed] != FIN_E) noFight=0;
        else noFight++;
		fin[m.ed]=fin[m.st];
		fin[m.st]=FIN_E;
		who^=1;
	} else {
		Flip(m.st);
		noFight=0;
	}
}
/*
int bittuhMoveGen(BOARD brd, MOVLST &lst) const{


}*/


