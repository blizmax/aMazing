#pragma once
#include"FSM.hpp"
#include"../dynamic_matrix.hpp"
namespace aMazing
{
	class DFA :public FSM
	{
	public:
		void minimize()
		{
			DynamicMatrix<bool> distinct(nodes.size(), nodes.size());
			//Initialize distinct array
			for (int i = 1; i < distinct.getWidth(); i++)
			{
				for (int j = 0; j < distinct.getHeight() - 1; j++)
				{
					if ((isFinal(nodes[i]->getIdentifer()) &&
						isFinal(nodes[j]->getIdentifer())) == false)
					{
						distinct[i][j] = true;
					}
					else
					{
						distinct[i][j] = false;
					}
				}
			}
			//Loop until no change is made.
			bool hasDistinctChange = true;
			while (hasDistinctChange)
			{
				hasDistinctChange = false;
				for (AlphaType a : alphabet)
				{
					std::vector<Bridge>& trans = transitions[a];
					for (int i = 1; i < distinct.getWidth(); i++)
					{
						int id_a = nodes[i]->getIdentifer();
						for (int j = 0; j < distinct.getHeight() - 1; j++)
						{
							int id_b = nodes[j]->getIdentifer();
							int tran_a = lookUpTransitions(id_a, a);
							int tran_b = lookUpTransitions(id_b, a);
							int index_tran_a = searchNode(tran_a);
							int index_tran_b = searchNode(tran_b);
							if (index_tran_a == -1 ||
								index_tran_b == -1)
							{
								throw std::exception("No transition result found");
							}
							if (distinct[i][j] == false &&
								distinct[index_tran_a][index_tran_b] == true)
							{
								distinct[i][j] = true;
								hasDistinctChange = true;
							}
						}
					}
				}
			}
			//adapt the result table.
			//when cell's vaule is false,the nodes that cell's column and row stands can be merged. 
			std::vector<std::pair<int,int> > idsToMerge;
			for (int i = 1; i < distinct.getWidth(); i++)
			{
				for (int j = 0; j < distinct.getHeight() - 1; ++j)
				{
					if (distinct[i][j] == false)
					{
						idsToMerge.push_back(std::pair<int, int>(nodes[i]->getIdentifer(),
							nodes[j]->getIdentifer()));
					}
				}
			}
			//Merge ids.
			for (int i = 0; i < idsToMerge.size(); i++)
			{
				if (idsToMerge[i].first == idsToMerge[i].second)
				{
					continue;
				}
				if (idsToMerge[i].second != startState)
				{
					std::vector<Bridge> asStart, asEnd;
					getNodeTransitions(idsToMerge[i].second, asStart, asEnd);
					for (Bridge bg : asStart)
					{
						bindA2B(idsToMerge[i].first, bg.end, bg.alpha);
					}
					for (Bridge bg : asEnd)
					{
						bindA2B(bg.start, idsToMerge[i].first, bg.alpha);
					}
					eraseNode(idsToMerge[i].second);
				}
				else
				{
					std::vector<Bridge> asStart, asEnd;
					getNodeTransitions(idsToMerge[i].first, asStart, asEnd);
					for (Bridge bg : asStart)
					{
						bindA2B(idsToMerge[i].second, bg.end, bg.alpha);
					}
					for (Bridge bg : asEnd)
					{
						bindA2B(bg.start, idsToMerge[i].second, bg.alpha);
					}
					eraseNode(idsToMerge[i].first);
				}
			}
			//Finish job.
		}
	};
}
