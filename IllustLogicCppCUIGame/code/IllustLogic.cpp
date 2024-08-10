#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <thread>
#include<algorithm>
#include "PoolAllocator.h"

using namespace std;

/* この実習で学んだ様々なものを使って簡単なCUIのゲームを作ってみよう。
 * 作成するものは何でもよいが下記の要件を満たす事。
 * タイトル ゲームシーケンス 結果表示が存在しそれらがステートマシンによってコントロールされていること（任意)
 * クラスの多態性を用いたオブジェクト管理が入っていること(任意)
 * 自分が作成した Poolアロケータが使われていること(必須)
 * 何かしらの処理にスレッドを用いた並行処理が入っていること(任意)
 * ゲームエンジンの使用は不可
 * 使用する言語は C ++
 */

// 問題
vector< vector<vector<char>>> puzzleList = {
	{
		{'#', '.' , '#'},
		{'.', '.' , '.'},
		{'#', '#' , '#'}
	},
	{
		{'.', '#' , '#' , '#', '.'},
		{'#', '#' , '.' , '#', '#'},
		{'#', '.' , '.' , '.', '#'},
		{'#', '#' , '.' , '#', '#'},
		{'.', '#' , '#' , '#', '.'}
	},
	{
		{'.', '.' , '#' , '.', '.'},
		{'#', '#' , '#' , '#', '.'},
		{'#', '#' , '#' , '#', '#'},
		{'#', '#' , '#' , '#', '.'},
		{'.', '.' , '#' , '.', '.'}
	},
	{
		{'#', '#' , '#' , '#', '#'},
		{'.', '.' , '.' , '#', '.'},
		{'.', '.' , '#' , '.', '.'},
		{'.', '#' , '.' , '.', '.'},
		{'#', '#' , '#' , '#', '#'}
	},
	{
		{'.', '.' ,'.', '#' , '.', '.'},
		{'.', '.' ,'.', '#' , '#', '#'},
		{'.', '#' ,'.', '#' , '.', '.'},
		{'.', '#' ,'.', '#' , '.', '.'},
		{'#', '#' ,'#', '#' , '#', '#'}
	},
	{
		{'#', '#', '#', '.', '.', '.', '.', '.' , '.', '.', '.'},
		{'#', '.', '.', '.', '.', '#', '.', '.' , ',', '#', '.'},
		{'#', '.', '.', '.', '#', '#', '#', '.' , '#', '#', '#'},
		{'#', '.', '.', '.', '.', '#', '.', '.' , ',', '#', '.'},
		{'#', '#', '#', '.', '.', '.', '.', '.' , ',', '.', '.'}
	},
	{
		{'#', '#', '#', '.', '.', '#', '.', '#' , '.'},
		{'#', '.', '.', '.', '#', '#', '#', '#' , '#'},
		{'#', '.', '.', '.', '.', '#', '.', '#' , '.'},
		{'#', '.', '.', '.', '#', '#', '#', '#' , '#'},
		{'#', '#', '#', '.', '.', '#', '.', '#' , ','}
	},
	{
		{'#', '#' , '.' , '.', '.','#', '.' , '.', '#','.','.','.', '#' , '.', '#','#', '.' , '.', '.','#', '.' , '.', '#','#', '#' },
		{'#', '.' , '#' , '.', '#','.', '#' , '.', '#','#','.','.', '#' , '.', '#','.', '#' , '.', '#','.', '#' , '.', '.','#', '.' },
		{'#', '#' , '.' , '.', '#','#', '#' , '.', '#','.','#','.', '#' , '.', '#','.', '#' , '.', '#','#', '#' , '.', '.','#', '.' },
		{'#', '.' , '#' , '.', '#','.', '#' , '.', '#','.','.','#', '#' , '.', '#','.', '#' , '.', '#','.', '#' , '.', '.','#', '.' },
		{'#', '#' , '#' , '.', '#','.', '#' , '.', '#','.','.','.', '#' , '.', '#','#', '.' , '.', '#','.', '#' , '.', '#','#', '#' }
	},
	{
		{'#','.','.','.', '#' , '.', '#','#','#', '#' , '.' , '#', '.','.','.','#', '.' , '#', '#','#','#', '.' , '#','#', '#','#'},
		{'#','#','.','.', '#' , '.', '#','.','.', '#' , '.' , '#', '#','.','#','#', '.' , '#', '.','.','.', '.' , '#','.', '.','#'},
		{'#','.','#','.', '#' , '.', '#','#','#', '#' , '.' , '#', '.','#','.','#', '.' , '#', '.','.','.', '.' , '#','.', '.','#'},
		{'#','.','.','#', '#' , '.', '#','.','.', '#' , '.' , '#', '.','.','.','#', '.' , '#', '.','.','.', '.' , '#','.', '.','#'},
		{'#','.','.','.', '#' , '.', '#','.','.', '#' , '.' , '#', '.','.','.','#', '.' , '#', '#','#','#', '.' , '#','#', '#','#'}
	},
	{
		{'#', '#' , '#' , '.', '#','#', '#' , '.', '#','.', '#' , '.', '#','#', '.' , '.', '#','#', '#' , '.', '.','#', '.', '.', '#', '#' , '#' },
		{'#', '.' , '.' , '.', '.','#', '.' , '.', '#','.', '#' , '.', '#','.', '#' , '.', '.','#', '.' , '.', '#','.', '#', '.', '#', '.' , '.' },
		{'.', '#' , '.' , '.', '.','#', '.' , '.', '#','.', '#' , '.', '#','.', '#' , '.', '.','#', '.' , '.', '#','.', '#', '.', '.', '#' , '.' },
		{'.', '.' , '#' , '.', '.','#', '.' , '.', '#','.', '#' , '.', '#','.', '#' , '.', '.','#', '.' , '.', '#','.', '#', '.', '.', '.' , '#' },
		{'#', '#' , '#' , '.', '.','#', '.' , '.', '.','#', '.' , '.', '#','#', '.' , '.', '#','#', '#' , '.', '.','#', '.', '.', '#', '#' , '#' }
	},
};

// 問題の答え
vector<string> puzzleIllustNameList = {
	"顔",
	"〇",
	"矢印",
	"Z",
	"止",
	"C++",
	"C#",
	"BANDAI",
	"NAMCO",
	"STUDIOS"
};

// クリアリスト
vector<int> puzzleClearIDList;

const int INT_ALLOCATOR_SIZE = 1;
PoolAllocator<int, INT_ALLOCATOR_SIZE> intAllocator;

#pragma region 色定義

#define		COL_BLACK	0x00
#define		COL_DARK_BLUE		0x01
#define		COL_DARK_GREEN	0x02
#define		COL_DARK_CYAN		0x03
#define		COL_DARK_RED		0x04
#define		COL_DARK_VIOLET	0x05
#define		COL_DARK_YELLOW	0x06
#define		COL_GRAY	0x07
#define		COL_LIGHT_GRAY		0x08
#define		COL_BLUE		0x09
#define		COL_GREEN	0x0a
#define		COL_CYAN		0x0b
#define		COL_RED		0x0c
#define		COL_VIOLET	0x0d
#define		COL_YELLOW	0x0e
#define		COL_WHITE	0x0f
#define		COL_INTENSITY		0x08		//	高輝度マスク
#define		COL_RED_MASK		0x04
#define		COL_GREEN_MASK	0x02
#define		COL_BLUE_MASK	0x01

//	文字色指定 for Windows Console
void setColor(int col)
{
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD attr = 0;
	if (col & COL_INTENSITY)
		attr |= FOREGROUND_INTENSITY;
	if (col & COL_RED_MASK)
		attr |= FOREGROUND_RED;
	if (col & COL_GREEN_MASK)
		attr |= FOREGROUND_GREEN;
	if (col & COL_BLUE_MASK)
		attr |= FOREGROUND_BLUE;
	SetConsoleTextAttribute(hCons, attr);
}
void setColor(int fg, int bg)
{
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD attr = 0;
	if (fg & COL_INTENSITY)
		attr |= FOREGROUND_INTENSITY;
	if (fg & COL_RED_MASK)
		attr |= FOREGROUND_RED;
	if (fg & COL_GREEN_MASK)
		attr |= FOREGROUND_GREEN;
	if (fg & COL_BLUE_MASK)
		attr |= FOREGROUND_BLUE;

	if (bg & COL_INTENSITY)
		attr |= BACKGROUND_INTENSITY;
	if (bg & COL_RED_MASK)
		attr |= BACKGROUND_RED;
	if (bg & COL_GREEN_MASK)
		attr |= BACKGROUND_GREEN;
	if (bg & COL_BLUE_MASK)
		attr |= BACKGROUND_BLUE;
	SetConsoleTextAttribute(hCons, attr);
}

#pragma endregion

struct Pos {
	Pos() : x(0), y(0) {}
	Pos(int _x, int _y) :x(_x), y(_y) {}

	int x;
	int y;
};

struct PuzzleInfo
{
	char answer;
	int backColor;
	bool isSign;
	Pos drawPos;

	PuzzleInfo() : answer('.'), backColor(COL_WHITE), isSign(false), drawPos(Pos(0, 0)) {}

	PuzzleInfo(char _answer, Pos _drawPos) : answer(_answer), backColor(COL_WHITE), isSign(false), drawPos(_drawPos) {

	}
};

#pragma region 描画
// 文字列の描画
void DrawCharPos(Pos _pos, string s)
{
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = _pos.x;
	pos.Y = _pos.y;
	SetConsoleCursorPosition(hCons, pos);

	cout << s;
}
// 文字の描画
void DrawCharPos(Pos _pos, char c)
{
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = _pos.x;
	pos.Y = _pos.y;
	SetConsoleCursorPosition(hCons, pos);

	cout << c;
}
// 文字色付きで文字を描画(背景色は直前に設定した値)
void DrawCharColorPos(Pos _pos, char c, int fg)
{
	//色を設定
	setColor(fg);

	//描画
	DrawCharPos(_pos, c);

	// 色をデフォルトに戻す
	setColor(COL_WHITE);
}
// 文字色・背景色付きで文字を描画
void DrawCharColorPos(Pos _pos, char c, int fg, int bg)
{
	//色を設定
	setColor(fg, bg);

	//描画
	DrawCharPos(_pos, c);

	// 色をデフォルトに戻す
	setColor(COL_WHITE, COL_BLACK);
}
#pragma endregion

// テスト用 正解の配置を描画
void DrawTrueTable(vector<vector<char>> puzzle) 
{
	for (int i = 0; i < puzzle.size(); i++)
	{
		for (int j = 0; j < puzzle.at(0).size(); j++)
		{
			setColor(COL_WHITE, COL_BLACK);

			if (puzzle.at(i).at(j) == '.') {
				setColor(COL_WHITE, COL_WHITE);
			}
			else
			{
				setColor(COL_WHITE, COL_GREEN);
			}
		}
		setColor(COL_WHITE, COL_BLACK);
	}
}

// パズル全体の描画、変更など
class ILPuzzle{

private:


#pragma region パズル(vector<vector<char>>)の解析

	//パズルの上側に表示するヒントのうち、左から数えてxの列を計算
	vector<int> PuzzleHintX(vector<vector<char>> puzzle, int x)
	{
		int cnt = 0;
		vector<int> thisXHint;

		for (int y = 0; y < puzzle.size(); y++)
		{
			char c = puzzle.at(y).at(x);

			if (c == '#') {
				++cnt;
			}
			else
			{
				if (cnt > 0) {
					thisXHint.push_back(cnt);
				}
				cnt = 0;
			}
		}

		// 残った#のcntを入れる
		if (cnt > 0) {
			thisXHint.push_back(cnt);
		}

		if (thisXHint.size() == 0) {
			thisXHint.push_back(0);
		}

		return thisXHint;
	}
	//パズルの上側に表示するヒントを計算
	vector<vector<int>> PuzzleHintXs(vector<vector<char>> puzzle)
	{
		vector<vector<int>> hintXs;

		for (int x = 0; x < puzzle.at(0).size(); x++)
		{
			hintXs.push_back(PuzzleHintX(puzzle, x));
		}

		return hintXs;
	}

	//パズルの左側に表示するヒントのうち、上から数えてyの列を計算
	vector<int> PuzzleHintY(vector<vector<char>> puzzle, int y)
	{
		int cnt = 0;
		vector<int> thisYHint;

		for (int x = 0; x < puzzle.at(0).size(); x++)
		{
			char c = puzzle.at(y).at(x);

			if (c == '#') {
				++cnt;
			}
			else
			{
				if (cnt > 0) {
					thisYHint.push_back(cnt);
				}
				cnt = 0;
			}
		}

		// 残った#のcntを入れる
		if (cnt > 0) {
			thisYHint.push_back(cnt);
		}

		if (thisYHint.size() == 0) {
			thisYHint.push_back(0);
		}

		return thisYHint;
	}
	//パズルの左側に表示するヒントを計算
	vector<vector<int>> PuzzleHintYs(vector<vector<char>> puzzle)
	{
		vector<vector<int>> hintYs;

		for (int y = 0; y < puzzle.size(); y++)
		{
			hintYs.push_back(PuzzleHintY(puzzle, y));
		}

		return hintYs;
	}

	//hintX,Yの最大要素数を計算
	Pos PuzzleMaxHintXY(vector<vector<int>> hintXs, vector<vector<int>> hintYs)
	{
		// hintXs中のhintXの最大要素数を計算する
		int maxHintX = 0;
		for (int i = 0; i < hintXs.size(); i++)
		{
			maxHintX = max(maxHintX, hintXs.at(i).size());
		}
		// hintYs中のhintYの最大要素数を計算する
		int maxHintY = 0;
		for (int i = 0; i < hintYs.size(); i++)
		{
			maxHintY = max(maxHintY, hintYs.at(i).size());
		}

		return Pos(maxHintX, maxHintY);
	}

	//maxHintXs,YsからoffsetPosを計算
	Pos PuzzleOffsetPos(Pos maxHintXY)
	{
		return Pos(maxHintXY.y * 2, maxHintXY.x + 1);
	}

	//offsetPosとパズル上のPosから描画Posを計算
	Pos PuzzlePos2DrawPos(Pos offsetPos, Pos puzzlePos)
	{
		return Pos(offsetPos.x + 2 * puzzlePos.x, offsetPos.y + 2 * puzzlePos.y);
	}
#pragma endregion

#pragma region パーツごとの描画

	// ヒント部分を描画
	void DrawHint()
	{
		// 上部分のヒントを描画
		// maxHintX-1 から上に上がりながら描画していく
		// 横の移動は PuzzlePos2DrawPos で取る

		for (int i = 0; i < hintXs.size(); i++)
		{
			for (int j = 0; j < hintXs.at(i).size(); j++)
			{
				Pos drawPosU(PuzzlePos2DrawPos(offsetPos, Pos(i, 0)).x, maxHintXY.x - 1 - j);
				DrawCharColorPos(drawPosU, hintXs.at(i).at(hintXs.at(i).size() - 1 - j) + '0', COL_WHITE, COL_BLACK);
			}
		}

		// 左部分のヒントを描画
		// 2*(maxHintY-1) から左に戻りながら描画していく

		for (size_t i = 0; i < hintYs.size(); i++)
		{
			for (size_t j = 0; j < hintYs.at(i).size(); j++)
			{
				Pos drawPosL(2 * (maxHintXY.y - 1) - (2 * j), PuzzlePos2DrawPos(offsetPos, Pos(0, i)).y);
				DrawCharColorPos(drawPosL, hintYs.at(i).at(hintYs.at(i).size() - 1 - j) + '0', COL_WHITE, COL_BLACK);
			}
		}
	}

	//マスを描画
	void DrawTable()
	{
		for (int y = 0; y < puzzleInfos.size(); y++)
		{
			for (int x = 0; x < puzzleInfos.at(0).size(); x++)
			{
				PuzzleInfo p = puzzleInfos.at(y).at(x);
				int fillColor = p.backColor;
				DrawCharColorPos(p.drawPos, ' ', COL_WHITE, fillColor);
			}
		}
	}

	//印を描画(背景色はマスに合わせる)
	void DrawSign()
	{
		for (int y = 0; y < puzzleInfos.size(); y++)
		{
			for (int x = 0; x < puzzleInfos.at(0).size(); x++)
			{
				PuzzleInfo p = puzzleInfos.at(y).at(x);

				if (p.isSign)
				{
					DrawCharColorPos(p.drawPos, 'x', COL_BLACK, p.backColor);
				}
				else
				{
					DrawCharColorPos(p.drawPos, ' ', COL_BLACK, p.backColor);
				}
			}
		}
	}

	//指定座標にカーソルを描画(背景色はマスに合わせる)
	void DrawCursol()
	{
		PuzzleInfo p = puzzleInfos.at(cursolPos.y).at(cursolPos.x);
		DrawCharColorPos(p.drawPos, '@', COL_BLUE, p.backColor);
	}

#pragma endregion

public:
	Pos puzzleSize;
	vector<vector<int>> hintXs;
	vector<vector<int>> hintYs;
	Pos maxHintXY;
	Pos offsetPos;
	Pos cursolPos;
	vector<vector<PuzzleInfo>> puzzleInfos;

	//デフォルトコンストラクタ
	ILPuzzle(){
		vector<vector<char>> puzzle = {};
		puzzleSize = Pos();

		hintXs = {};
		hintYs = {};

		maxHintXY = Pos();
		offsetPos = Pos();
		cursolPos = Pos();

		puzzleInfos = {};
	}

	ILPuzzle(vector<vector<char>> _puzzle) {
		vector<vector<char>> puzzle = _puzzle;
		puzzleSize = Pos(puzzle.at(0).size(), puzzle.size());

		hintXs = PuzzleHintXs(puzzle);
		hintYs = PuzzleHintYs(puzzle);

		maxHintXY = PuzzleMaxHintXY(hintXs, hintYs);
		offsetPos = PuzzleOffsetPos(maxHintXY);
		cursolPos = Pos();

		puzzleInfos = vector<vector<PuzzleInfo>>(puzzleSize.y, vector<PuzzleInfo>(puzzleSize.x));
		for (int y = 0; y < puzzleSize.y; y++)
		{
			for (int x = 0; x < puzzleSize.x; x++)
			{
				PuzzleInfo _p(puzzle.at(y).at(x), PuzzlePos2DrawPos(offsetPos, Pos(x, y)));
				puzzleInfos.at(y).at(x) = _p;
			}
		}
	}

	//Puzzle全体(ヒント、マス、印)を描画する
	void DrawILPuzzle() {
		//ヒントを描画
		DrawHint();
		// パズルを描画
		DrawTable();
		// 印を描画
		DrawSign();
		// カーソルを描画
		DrawCursol();
	}

	//白いマスを黒くし、カーソル・印・ヒントを消すことで、塗ったマスが目立つようにして描画(Resultで使用)
	void RevealFillIllust() {

		//白いマスを黒くする
		for (int y = 0; y < puzzleSize.y; y++)
		{
			for (int x = 0; x < puzzleSize.x; x++)
			{
				if (puzzleInfos.at(y).at(x).backColor != COL_GREEN) {
					puzzleInfos.at(y).at(x).backColor = COL_BLACK;
				}
			}
		}

		// コンソールをクリア
		system("cls");

		// マスのみ描画
		DrawTable();
	}

	
#pragma region パズルの操作
	//カーソル座標のマスの色を反転する
	void FillTableAtCursol() {
		if (puzzleInfos.at(cursolPos.y).at(cursolPos.x).backColor == COL_GREEN)
		{
			puzzleInfos.at(cursolPos.y).at(cursolPos.x).backColor = COL_WHITE;
		}
		else
		{
			puzzleInfos.at(cursolPos.y).at(cursolPos.x).backColor = COL_GREEN;
		}
	}

	//指定座標の印を反転する
	void SignAtCursol() {

		if (puzzleInfos.at(cursolPos.y).at(cursolPos.x).isSign == false)
		{
			puzzleInfos.at(cursolPos.y).at(cursolPos.x).isSign = true;
		}
		else
		{
			puzzleInfos.at(cursolPos.y).at(cursolPos.x).isSign = false;
		}
	}

	//カーソルを移動する
	void MoveCursol(Pos direction) {
		if (direction.y == -1) {
			cursolPos.y = ((cursolPos.y + 1) % puzzleSize.y);
		}
		if (direction.x == -1) {
			cursolPos.x = ((cursolPos.x - 1 + puzzleSize.x) % puzzleSize.x);
		}
		if (direction.x == 1) {
			cursolPos.x = ((cursolPos.x + 1) % puzzleSize.x);
		}
		if (direction.y == 1) {
			cursolPos.y = ((cursolPos.y - 1 + puzzleSize.y) % puzzleSize.y);
		}
	}

#pragma endregion

	//クリアチェック
	bool CheckPuzzleClear()
	{
		//puzzle.answerとpuzzle.backColorを照らし合わせて正誤判定
		int clearCnt = 0;
		for (int y = 0; y < puzzleSize.y; y++)
		{
			for (int x = 0; x < puzzleSize.x; x++)
			{
				PuzzleInfo p = puzzleInfos.at(y).at(x);
				if ((p.answer != '#' && p.backColor == COL_WHITE) ||
					(p.answer == '#' && p.backColor == COL_GREEN)) {
					clearCnt++;
				}
			}
		}

		if (clearCnt == puzzleSize.x * puzzleSize.y) {
			return true;
		}
		return false;
	}
};


#pragma region ゲームのステート

enum StateType
{
	State_Title,
	State_Help,
	State_Select,
	State_Game,
	State_Result,
	State_End,
};

// 前方宣言
class State;
class TitleState;
class HelpState;
class SelectState;
class GameState;
class ResultState;


// ステートパターンでステートマシンを管理
class State {
private:

public:
	virtual ~State() = default;
	virtual unique_ptr<State> StartState() = 0;
};

class TitleState : public State {
private:

public:
	TitleState(){}

	unique_ptr<State> StartState() override;
};

class HelpState : public State {
private:

	// Help画面 説明用 一列パズル描画
	void DrawHelpExample(vector<int> colorList) {

		if (colorList.size() != 5) return;

		setColor(COL_WHITE, COL_BLACK);
		cout << "1 2 ";
		setColor(COL_WHITE, colorList.at(0));
		cout << " ";
		setColor(COL_WHITE, COL_BLACK);
		cout << " ";
		setColor(COL_WHITE, colorList.at(1));
		cout << " ";
		setColor(COL_WHITE, COL_BLACK);
		cout << " ";
		setColor(COL_WHITE, colorList.at(2));
		cout << " ";
		setColor(COL_WHITE, COL_BLACK);
		cout << " ";
		setColor(COL_WHITE, colorList.at(3));
		cout << " ";
		setColor(COL_WHITE, COL_BLACK);
		cout << " ";
		setColor(COL_WHITE, colorList.at(4));
		cout << " " << endl;
		setColor(COL_WHITE, COL_BLACK);
	}


	// Help画面のパズルの描画とキー操作をまとめたもの
	void DrawHelpPuzzle(ILPuzzle iLPuzzle)
	{
		system("cls");
		iLPuzzle.DrawILPuzzle();
		DrawCharPos(Pos(0, iLPuzzle.offsetPos.y + iLPuzzle.puzzleSize.y + 3), "正しく塗りつぶすことができればゲームクリアです。");
		cout << endl << endl << "キー入力で次へ" << endl << endl;
		_getch();
	}


public:
	HelpState() {

	}

	unique_ptr<State> StartState() override;
};

class SelectState : public State {

public:
	SelectState(){}

	unique_ptr<State> StartState() override;
};

class GameState : public State {

private:
	int* gameLevelPtr;
	ILPuzzle iLPuzzle;

	// ヘルプ部分を描画
	void DrawHelp()
	{
		DrawCharPos(Pos(0, iLPuzzle.maxHintXY.x + iLPuzzle.puzzleSize.y * 2 + 3), "イラストロジック LEVEL ");
		cout << *gameLevelPtr;  
		DrawCharPos(Pos(0, iLPuzzle.maxHintXY.x + iLPuzzle.puzzleSize.y * 2 + 5), "操作方法 :");
		DrawCharPos(Pos(0, iLPuzzle.maxHintXY.x + iLPuzzle.puzzleSize.y * 2 + 6), " [W] [A] [S] [D] : カーソル[@]の移動");
		DrawCharPos(Pos(0, iLPuzzle.maxHintXY.x + iLPuzzle.puzzleSize.y * 2 + 7), " [P] : マスを塗る");
		DrawCharPos(Pos(0, iLPuzzle.maxHintXY.x + iLPuzzle.puzzleSize.y * 2 + 8), " [X] : x印を付ける");
		DrawCharPos(Pos(0, iLPuzzle.maxHintXY.x + iLPuzzle.puzzleSize.y * 2 + 9), " [Q] : タイトルに戻る");
	}

public:
	GameState(int* _gameLevelPtr) {
		gameLevelPtr = _gameLevelPtr;
		vector<vector<char>> puzzle = puzzleList.at(*gameLevelPtr);
		iLPuzzle = ILPuzzle(puzzle);
	}

	unique_ptr<State> StartState() override;
};

class ResultState : public State {
private:
	int* gameLevelPtr;
	ILPuzzle iLPuzzle;

public:
	ResultState(int* _gameLevelPtr ,ILPuzzle _iLPuzzle) {
		gameLevelPtr = _gameLevelPtr;
		iLPuzzle = _iLPuzzle;
	}

	unique_ptr<State> StartState() override;
};


unique_ptr<State> TitleState::StartState(){
	// コンソールをクリア
	system("cls");

#pragma region タイトル表示
	// タイトル表示
	setColor(COL_WHITE, COL_DARK_RED);cout << "||||||||||" << endl;

	setColor(COL_CYAN, COL_RED);cout << "イ";
	setColor(COL_RED, COL_BLACK);cout << "------";
	setColor(COL_CYAN, COL_RED);cout << "イ" << endl;

	setColor(COL_BLUE, COL_DARK_YELLOW);cout << "ラ";
	setColor(COL_DARK_YELLOW, COL_BLACK);cout << "------";
	setColor(COL_BLUE, COL_DARK_YELLOW);cout << "ラ" << endl;

	setColor(COL_DARK_BLUE, COL_YELLOW);cout << "ス";
	setColor(COL_YELLOW, COL_BLACK);cout << "------";
	setColor(COL_DARK_BLUE, COL_YELLOW);cout << "ス" << endl;

	setColor(COL_DARK_VIOLET, COL_GREEN);cout << "ト";
	setColor(COL_GREEN, COL_BLACK);cout << "------";
	setColor(COL_DARK_VIOLET, COL_GREEN);cout << "ト" << endl;

	setColor(COL_RED, COL_CYAN);cout << "ロ";
	setColor(COL_CYAN, COL_BLACK);cout << "------";
	setColor(COL_RED, COL_CYAN);cout << "ロ" << endl;

	setColor(COL_DARK_YELLOW, COL_BLUE);cout << "ジ";
	setColor(COL_BLUE, COL_BLACK);cout << "------";
	setColor(COL_DARK_YELLOW, COL_BLUE);cout << "ジ" << endl;

	setColor(COL_YELLOW, COL_DARK_BLUE);	cout << "ッ";
	setColor(COL_DARK_BLUE, COL_BLACK);cout << "------";
	setColor(COL_YELLOW, COL_DARK_BLUE);	cout << "ッ" << endl;

	setColor(COL_GREEN, COL_DARK_VIOLET);cout << "ク";
	setColor(COL_DARK_VIOLET, COL_BLACK);cout << "------";
	setColor(COL_GREEN, COL_DARK_VIOLET);cout << "ク" << endl;

	setColor(COL_WHITE, COL_DARK_RED);cout << "||||||||||" << endl;
	setColor(COL_WHITE, COL_BLACK);
#pragma endregion

	// 次の操作を表示
	cout << endl;
	cout << "'h' - 遊び方へ" << endl;
	cout << "'s' - 問題選択へ" << endl;
	cout << "'q' - ゲームを終了" << endl;


	int i; // キー入力値
	while (true)
	{
		i = _getch(); // キー入力待ち

		switch (i) {
		case 'h':
			// 遊び方へ
		{
			return unique_ptr<State>(new HelpState());
		}
		case 's':
		{
			// 選択画面へ
			return unique_ptr<State>(new SelectState());
		}
		case 'q':
			// プログラムを終了
			return nullptr;
		}
	}
}

unique_ptr<State> HelpState::StartState(){

	/*
	上側、左側に書かれている数字は、その列をそれだけ連続して塗りつぶすかを表しています
	例えば　左に " 1 2 "と書かれている場合、その列は　□■□■■や■□□■■や■□■■□ のように塗られます。
	今回の例だと、□□□■□のマスはどのパターンでも塗られているため、確定で塗ることができます。
	正しく塗りつぶすことができればゲームクリアです。
	
	*/

	//3x3のパズルを解く実例を見せる
	vector<vector<char> > puzzle =
	{
		{'#', '.' , '#'},
		{'.', '.' , '.'},
		{'#', '#' , '#'}
	};
	ILPuzzle iLPuzzle = ILPuzzle(puzzle);


	#pragma region ゲームの概要を説明

	system("cls");
	cout << "遊び方　キー入力で次へ" << endl << endl;;
	cout << "このゲームは与えれた数字を基にマスを塗り、ドット絵を完成させるゲームです" << endl << endl;
	_getch();

	system("cls");
	iLPuzzle.DrawILPuzzle();
	DrawCharPos(Pos(0, iLPuzzle.offsetPos.y + iLPuzzle.puzzleSize.y + 3), "実際のゲーム画面です。\n上・左側に書かれている数字は、その列をそれだけ連続して塗りつぶすかを表しています");
	cout << endl << endl << "キー入力で次へ" << endl << endl;
	_getch();
	#pragma endregion
	
	#pragma region 塗り方の例
	// コンソールをクリア
	system("cls");
	cout << "遊び方　キー入力で次へ" << endl << endl;
	cout << "例えば　左に  1 2 と書かれている場合、" << endl << endl;
	DrawHelpExample({ COL_WHITE, COL_WHITE ,COL_WHITE ,COL_WHITE ,COL_WHITE });
	_getch();


	// コンソールをクリア
	system("cls");
	cout << "遊び方　キー入力で次へ" << endl << endl;
	cout << "その列は以下のいずれかで塗られます。" << endl << endl;
	DrawHelpExample({ COL_WHITE, COL_GREEN ,COL_WHITE ,COL_GREEN ,COL_GREEN });
	cout << endl << endl;
	DrawHelpExample({ COL_GREEN, COL_WHITE ,COL_WHITE ,COL_GREEN ,COL_GREEN });
	cout << endl << endl;
	DrawHelpExample({ COL_GREEN, COL_WHITE ,COL_GREEN ,COL_GREEN ,COL_WHITE });
	_getch();

	// コンソールをクリア
	system("cls");
	cout << "遊び方　キー入力で次へ" << endl << endl;
	cout << "今回の例だと、右から2番目のマスがどのパターンでも塗られているため、確定で塗ることができます。" << endl << endl;
	DrawHelpExample({ COL_WHITE, COL_GREEN ,COL_WHITE ,COL_BLUE ,COL_GREEN });
	cout << endl << endl;
	DrawHelpExample({ COL_GREEN, COL_WHITE ,COL_WHITE ,COL_BLUE ,COL_GREEN });
	cout << endl << endl;
	DrawHelpExample({ COL_GREEN, COL_WHITE ,COL_GREEN ,	COL_BLUE ,COL_WHITE });
	_getch();
	#pragma endregion
	
	#pragma region 3x3のパズルを解く実例を見せる

	DrawHelpPuzzle(iLPuzzle);

	iLPuzzle.puzzleInfos.at(2).at(0).backColor = COL_GREEN;
	DrawHelpPuzzle(iLPuzzle);

	iLPuzzle.puzzleInfos.at(2).at(1).backColor = COL_GREEN;
	DrawHelpPuzzle(iLPuzzle);

	iLPuzzle.puzzleInfos.at(2).at(2).backColor = COL_GREEN;
	DrawHelpPuzzle(iLPuzzle);

	iLPuzzle.puzzleInfos.at(1).at(0).isSign = true;
	DrawHelpPuzzle(iLPuzzle);

	iLPuzzle.puzzleInfos.at(1).at(1).isSign = true;
	DrawHelpPuzzle(iLPuzzle);

	iLPuzzle.puzzleInfos.at(1).at(2).isSign = true;
	DrawHelpPuzzle(iLPuzzle);

	iLPuzzle.puzzleInfos.at(0).at(0).backColor = COL_GREEN;
	DrawHelpPuzzle(iLPuzzle);

	iLPuzzle.puzzleInfos.at(0).at(2).backColor = COL_GREEN;
	DrawHelpPuzzle(iLPuzzle);

	iLPuzzle.puzzleInfos.at(0).at(1).isSign = true;
	DrawHelpPuzzle(iLPuzzle);
	#pragma endregion


	// コンソールをクリア
	system("cls");
	cout << "キー入力でタイトルに戻る" << endl << endl;
	_getch();
	
	return unique_ptr<State>(new TitleState());

}

unique_ptr<State> SelectState::StartState(){
	// コンソールをクリア
	system("cls");

	cout << "Select画面" << endl;
	cout << "'0'～'"<< (puzzleList.size() - 1) <<"'を入力して難易度を選択してください（ 0:EASY, " << (puzzleList.size() - 1) << ":HARD ）" << endl;

	// クリアした問題の答えを表示
	//昇順ソート
	sort(puzzleClearIDList.begin(), puzzleClearIDList.end());
	cout << endl << "クリア済の問題:" << endl;
	for (int i = 0; i < puzzleClearIDList.size(); i++)
	{
		int clearId = puzzleClearIDList.at(i);
		cout << endl << "LEVEL " << clearId << " : " << puzzleIllustNameList.at(clearId) << endl;
	}


	int i; // キー入力値
	while (true) {
		i = _getch(); // キー入力待ち

		if (i >= '0' && i <= '0' + puzzleList.size() - 1) {
			int* gameLevelPtr = intAllocator.Alloc();
			*gameLevelPtr = i - '0';
			return unique_ptr<State>(new GameState(gameLevelPtr));
		}

	}
}

unique_ptr<State> GameState::StartState(){
	// コンソールをクリア
	system("cls");

	// エスケープシーケンスを有効に
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hStdOut, &mode);
	SetConsoleMode(hStdOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	// カーソルを消す
	printf("\x1b[?25l");

	// カーソルの座標
	Pos cursolPos(0, 0);

	//パズルを描画
	iLPuzzle.DrawILPuzzle();
	// ヘルプを描画
	DrawHelp();

	int i; // キー入力値

	while (true)
	{
		i = _getch(); // キー入力待ち

		switch (i) {
		case 'q':
		{
			// タイトルに戻る
			intAllocator.Free(gameLevelPtr);
			return unique_ptr<State>(new TitleState());
		}
		case 's':
			iLPuzzle.MoveCursol(Pos(0, -1));
			break;
		case 'a':
			iLPuzzle.MoveCursol(Pos(-1, 0));
			break;
		case 'd':
			iLPuzzle.MoveCursol(Pos(1, 0));
			break;
		case 'w':
			iLPuzzle.MoveCursol(Pos(0, 1));
			break;
		case 'p':
			iLPuzzle.FillTableAtCursol();
			break;
		case 'x':
			iLPuzzle.SignAtCursol();
			break;
		}

		iLPuzzle.DrawILPuzzle();

		// 正誤判定
		if (iLPuzzle.CheckPuzzleClear()) {
			// Resultに移動
			return unique_ptr<State>(new ResultState(gameLevelPtr ,iLPuzzle));
		}
	}


}

unique_ptr<State> ResultState::StartState()
{
	// パズル部分以外をクリア
	iLPuzzle.RevealFillIllust();


	// クリア表示
	setColor(COL_YELLOW);
	DrawCharPos(Pos(0, iLPuzzle.offsetPos.y + iLPuzzle.puzzleSize.y * 2 + 3), "Game Clear!!");
	setColor(COL_WHITE);
	cout << endl <<  "'q' - タイトルに戻る" <<endl;

	//クリアリストに追加
	bool isAlreadyClear = std::find(puzzleClearIDList.begin(), puzzleClearIDList.end(), *gameLevelPtr) != puzzleClearIDList.end();
	if (!isAlreadyClear) {
		puzzleClearIDList.push_back(*gameLevelPtr);
	}

	int i; // キー入力値
	while (true)
	{
		i = _getch(); // キー入力待ち

		switch (i) {
		case 'q':
			intAllocator.Free(gameLevelPtr);
			return unique_ptr<State>(new TitleState());
		}
	}
}


#pragma endregion


int main()
{
	//カーソルを消す
	printf("\x1b[?25l");

	StateType stateType = State_Title;

	unique_ptr<State> state = make_unique<TitleState>();
	
	while (true)
	{
		state = state->StartState();

		if (state == nullptr)break;
	}

	return EXIT_SUCCESS;
}