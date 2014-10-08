#include "MazeGenerator.h"


MazeGenerator* MazeGenerator::instance = nullptr;

MazeGenerator::MazeGenerator()
{
	srand(time(nullptr));
}


MazeGenerator::~MazeGenerator()
{
}

MazeGenerator& MazeGenerator::getInstance()
{
	if (instance == nullptr)
	{
		instance = new MazeGenerator;
	}
	return *instance;
}

void MazeGenerator::genMazeRecuresion(Maze* result, int px, int py)
{
	int dir[4] = { 0, 1, 2, 3 };
	int loop_times = rand() % 10;
	for (int i = 0; i < loop_times; i++)
	{
		int f = rand() % 4;
		int s = rand() % 4;
		int t = dir[f];
		dir[f] = dir[s];
		dir[s] = t;
	}
	for (auto d : dir)
	{
		int pos_x = px;
		int pos_y = py;
		switch (d)
		{
		case 0:
			px += 1;
			if (checkPoint(result, px, py) && (result->get(px, py) != 0) &&
				checkPoint(result, px + 1, py) && (result->get(px + 1, py) != 0) &&
				checkPoint(result, px, py + 1) && (result->get(px, py + 1) != 0) &&
				checkPoint(result, px, py - 1) && (result->get(px, py - 1) != 0))
			{
				result->get(px, py) = 0;
				genMazeRecuresion(result, px, py);
			}
			break;
		case 1:
			px -= 1;
			if (checkPoint(result, px, py) && (result->get(px, py) != 0) &&
				checkPoint(result, px - 1, py) && (result->get(px - 1, py) != 0) &&
				checkPoint(result, px, py + 1) && (result->get(px, py + 1) != 0) &&
				checkPoint(result, px, py - 1) && (result->get(px, py - 1) != 0))
			{
				result->get(px, py) = 0;
				genMazeRecuresion(result, px, py);
			}
			break;
		case 2:
			py += 1;
			if (checkPoint(result, px, py) && (result->get(px, py) != 0) &&
				checkPoint(result, px, py + 1) && (result->get(px, py + 1) != 0) &&
				checkPoint(result, px + 1, py) && (result->get(px + 1, py) != 0) &&
				checkPoint(result, px - 1, py) && (result->get(px - 1, py) != 0))
			{
				result->get(px, py) = 0;
				genMazeRecuresion(result, px, py);
			}
			break;
		case 3:
			py -= 1;
			if (checkPoint(result, px, py) && (result->get(px, py) != 0) &&
				checkPoint(result, px, py - 1) && (result->get(px, py - 1) != 0) &&
				checkPoint(result, px + 1, py) && (result->get(px + 1, py) != 0) &&
				checkPoint(result, px - 1, py) && (result->get(px - 1, py) != 0))
			{
				result->get(px, py) = 0;
				genMazeRecuresion(result, px, py);
			}
			break;
		default:
			break;
		}
		px = pos_x;
		py = pos_y;
	}
}

Maze* MazeGenerator::genMaze(int dimension,CollisionWorld* collision)
//get a new Maze generated by MazeGenerator
{
	Maze* output = nullptr;
	output = new Maze;
	if (output == nullptr)
	{
		return nullptr;
	}
	output->width = dimension;
	output->height = dimension;
	output->m_map = new bool[dimension * dimension];
	for (int i = 0; i<dimension * dimension; i++)
	{
		output->m_map[i] = true;
	}

	output->get(dimension / 2, dimension / 2) = 0;
	genMazeRecuresion(output, dimension / 2, dimension / 2);


	//register maze Blocks in collision world
	float xOffSet = Maze::blockSize * output->width / 2.0f;
	float yOffSet = Maze::blockSize * output->height / 2.0f;

	for (int i = 0; i < output->height; i++)
	{
		for (int j = 0; j < output->width; j++)
		{
			if (output->get(i,j) == 1)
			{
				collision->addBox(i * Maze::blockSize - xOffSet,
					j * Maze::blockSize - yOffSet,
					Maze::blockSize / 2.0f,
					Maze::blockSize / 2.0f);
			}
		}
	}
	return output;
}

bool MazeGenerator::checkPoint(Maze* ref,const std::pair<int, int>& pt)
{
	if (pt.first >= 0 && pt.second >= 0)
	{
		if (pt.first < ref->width && (pt.second < ref->height))
		{
			return true;
		}
	}
	return false;
}


bool MazeGenerator::checkPoint(Maze* ref, int a, int b)
{
	if (a >= 0 && b >= 0)
	{
		if (a < ref->width && (b < ref->height))
		{
			return true;
		}
	}
	return false;
}
