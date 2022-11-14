﻿#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <conio.h>
#include <algorithm>

#define DEBUG
//#define DEBUG_K5
#define DEBUG_K33
//#define DEBUG2

using namespace std;

void CleanVector(vector<int>& inp);

// СТРУКТУРЫ

// Структура вершины: Имя, Список инцидентности (СИ), Функция вывода своего СИ
struct vertex
{
	string name;
	vector<int> IncidenceList;

	void DisplayInfo() {
		cout << "For vertex " << name << ": ";
		for (int i = 0; i < IncidenceList.size(); i++) cout << IncidenceList[i] << " ";
		cout << "\n";
	}
	int degree() {
		return IncidenceList.size();
	}
};

// Структура графа: Список вершин, Функция вывода СИ всех своих вершин
struct graph
{
	vector<vertex> Vertices;

	void DisplayAllIncidenceList() {
		for (int i = 0; i < Vertices.size(); i++) Vertices[i].DisplayInfo();
	}
	bool empty() {
		return !Vertices.size();
	}
	int edgesCount() {
		vector<int> edges;
		for (int i = 0; i < Vertices.size(); i++) {
			for (int k = 0; k < Vertices[i].IncidenceList.size(); k++) {
				edges.push_back(Vertices[i].IncidenceList[k]);
			}
		}
		CleanVector(edges);
		return edges.size();
	}
};

// ФУНКЦИИ

// Парсинг строки как запись вершины
vertex GetVertexFromString(string& str) { 
	vertex Output;

	Output.name = str.substr(0, str.find(" ")); // Парсинг имени вершины
	str.erase(0, Output.name.size() + 1);

	istringstream currentstring(str); 
	string temp;
	while (getline(currentstring, temp, ' ')) { // Парсинг списка инцидентности
		try {
			Output.IncidenceList.push_back(stoi(temp));
		}
		catch (...) {
			cout << "Ingnored invalid edge number input!\n";
		}
	}
	return Output;
}

// Парсинг графа из файла
graph GetGraphFromFile(string filename) { 
	ifstream input(filename);
	if (!input.is_open()) {
		cout << "Error opening file, try again.\n";
		return {};
	}

	graph Output;
	string temp;
	while (getline(input, temp)) 	Output.Vertices.push_back(GetVertexFromString(temp));
	input.close();

	cout << "Opened file " << filename << "\n";
	return Output;
}

// Запись графа в файл
void WriteGraphToFile(graph& inpG, string filename) { 
	if (inpG.empty()) {
		cout << "Tried to write empty graph, aborting\n";
		return;
	}

	ofstream fout(filename);
	if (!fout.is_open()) {
		cout << "Error accesing specified file, try again.\n";
		return;
	}

	for (int i = 0; i < inpG.Vertices.size(); i++) {
		fout << inpG.Vertices[i].name; // Запись имени вершин
		if (inpG.Vertices[i].IncidenceList.empty()) { fout << '\n'; continue; }
		fout << ' ';
		for (int k = 0; k < inpG.Vertices[i].IncidenceList.size(); k++) {
			fout << inpG.Vertices[i].IncidenceList[k];						// Запись СИ
			if (k != inpG.Vertices[i].IncidenceList.size()-1) fout << ' ';	// В частности пробелов между элементами списка
		}
		/*if (i != inp.Vertices.size() - 1)*/ fout << '\n'; // Переходим на новую строку после каждой вершины кроме последней (опционально)
	}

	fout.close();
	cout << "Graph succesfully saved in file " << filename << "\n";
}

// Чистка вектора от дубликатов
void CleanVector(vector<int>& inp) {
	auto end = inp.end();
	for (auto it = inp.begin(); it != end; ++it) end = remove(it + 1, end, *it);
	inp.erase(end, inp.end());
}

// Получение номеров смежных вершин для заданной в графе в данный список
void GetNeighboorVertices(graph& inpG, int inpV, vector<int>& NeighboorArray) { 
	for (int i = 0; i < inpG.Vertices.size(); i++) {
		if (i == inpV) continue;
		for (int k = 0; k < inpG.Vertices[i].IncidenceList.size(); k++) {
			if (count(inpG.Vertices[inpV].IncidenceList.begin(), inpG.Vertices[inpV].IncidenceList.end(), inpG.Vertices[i].IncidenceList[k])) {
				NeighboorArray.push_back(i);
			}
		}
	}
}

// Проверка, являются ли данные вершины смежными
bool Neighboors(graph& inpG, int inpV1, int inpV2) {
	vector<int> NeighboorCheck;
	GetNeighboorVertices(inpG, inpV1, NeighboorCheck);
	return count(NeighboorCheck.begin(), NeighboorCheck.end(), inpV2); // Проходим по массиву соседей вершины 1 и ищем там вершину 2
}

// Проверка на возможность исключения вершины без нарушения гомеоморфизма исходному графу
bool CanBeExcluded(graph& inpG, int inpV) { 
	vector<int> check;
	GetNeighboorVertices(inpG, inpV, check);
	if (check.size() != 2 || Neighboors(inpG, check[0], check[1])) return false; // Если степень вершины не 2, то исключить нельзя
																				// Если две смежные вершины данной - смежны, тоже исключить нельзя (цикл)
	return true;
}

// Получение номеров общих вершин данных графов (нумерация по первому графу)
void GetCommonVertices(graph& inpG1, graph& inpG2, vector<int>& commonVertices) {
	for (int i = 0; i < inpG1.Vertices.size(); i++) {
		for (int k = 0; k < inpG2.Vertices.size(); k++) {
			if (inpG1.Vertices[i].name == inpG2.Vertices[k].name) {
				//sort(inpG1.Vertices[i].IncidenceList.begin(), inpG1.Vertices[i].IncidenceList.end()); // Если для поиска общих вершин также нужно учитывать и их СИ
				//sort(inpG2.Vertices[k].IncidenceList.begin(), inpG2.Vertices[k].IncidenceList.end());
				//if (inpG1.Vertices[i].IncidenceList == inpG2.Vertices[k].IncidenceList) {
				commonVertices.push_back(i);
#ifdef DEBUG
				cout << "Found common vertex " << inpG1.Vertices[i].name << "\n";
#endif // DEBUG
				//}
			}
		}
	}
#ifdef DEBUG
	if (!commonVertices.size()) cout << "No common vertices found.\n";
	else cout << "Found " << commonVertices.size() << " common vertices\n\n";
#endif // DEBUG
}

// Получение имён рёбер, инцидентных данным вершинам
void GetCommonEdges(vertex& inpV1, vertex inpV2, vector<int>& commonEdgesArray) {
	for (int i = 0; i < inpV1.IncidenceList.size(); i++) {
		for (int k = 0; k < inpV2.IncidenceList.size(); k++) {
			if (inpV1.IncidenceList[i] == inpV2.IncidenceList[k]) {
				commonEdgesArray.push_back(inpV1.IncidenceList[i]);
			}
		}
	}
}

// Получение имён общих рёбер данных графов
void GetCommonEdges(graph& inpG1, graph& inpG2, vector<int>& commonEdgesArray) {
	if (inpG1.empty() || inpG2.empty()) return; // Если хотя бы один входной граф пустой, не тратим драгоценные ресурсы компутера
	vector<int> commonVertices;
	GetCommonVertices(inpG1, inpG2, commonVertices);
	for (int i=0; i < commonVertices.size(); i++) {
		for (int k = 0; k < commonVertices.size(); k++) {
			GetCommonEdges(inpG1.Vertices[commonVertices[i]], inpG2.Vertices[commonVertices[k]], commonEdgesArray);
		}
	}
#ifdef DEBUG
	if (!commonEdgesArray.size()) {
		cout << "No common edges found.\n";
		return;
	}
#endif // DEBUG
	CleanVector(commonEdgesArray); // Чистка от дубликатов
#ifdef DEBUG
	for (int i = 0; i < commonEdgesArray.size(); i++) {
		cout << "Found common edge " << commonEdgesArray[i] << "\n";
	}
	cout << "Found " << commonEdgesArray.size() << " common edges\n\n";
#endif // DEBUG
}

// Получение номера [из СИ данной вершины] ребра по его имени.
int GetEdgeNumber(vertex& inpV, int edgeName) { 
	for (int i = 0; i < inpV.IncidenceList.size(); i++) {
		if (inpV.IncidenceList[i] == edgeName) return i;
	}
#ifdef DEBUG
	cout << "No match for edge " << edgeName << " and vertex " << inpV.name << "\n";
#endif // DEBUG
	return 0;
}

// Исключение [не нарушающее гомеоморфизм исходному графу] вершины из данного графа. [Опционально] ---TODO: сделать выбор какое именно ребро сохранять при исключении
bool ExcludeVertex(graph& inpG, int inpV) { 
	if (CanBeExcluded(inpG, inpV)) {
		vector<int> neighboors;
		GetNeighboorVertices(inpG, inpV, neighboors);

		vector<int> commonEdges;
		GetCommonEdges(inpG.Vertices[inpV], inpG.Vertices[neighboors[0]], commonEdges); // Запись в СИ первого соседа ребра, инцидентного удаляемой вершине и второму соседу
		inpG.Vertices[neighboors[1]].IncidenceList.push_back(commonEdges[0]);

		commonEdges.clear();
		GetCommonEdges(inpG.Vertices[neighboors[1]], inpG.Vertices[inpV], commonEdges); // Удаление из СИ второго соседа ребра, инцидентного с ним и удаляемой вершиной
		inpG.Vertices[neighboors[1]].IncidenceList.erase(inpG.Vertices[neighboors[1]].IncidenceList.begin() + GetEdgeNumber(inpG.Vertices[neighboors[1]], commonEdges[0]));

#ifdef DEBUG2
		cout << "Excluding vertex " << inpG.Vertices[inpV].name << ".\tDone.\n";
#endif
		inpG.Vertices.erase(inpG.Vertices.begin()+inpV);
		return 1;
	}
	else return 0;
}

// Исключение всех возможных вершин из данного графа
void ExcludeAllVertices(graph& inpG) { 
	bool isReady;
	do {
		for (int i = 0; i < inpG.Vertices.size(); ) {
			if (!ExcludeVertex(inpG, i)) i++; // Удаление всех возможных вершин
		}

		isReady = true;
		for (int i = 0; i < inpG.Vertices.size(); i++) { // Проверка все ли удалено
			CanBeExcluded(inpG, i) ? isReady = false: NULL;
		}
	} while (!isReady); 
}

// Удаление вершины и инцидентных ей ребер из графа
void DeleteVertex(graph& inpG, int inpV) { 
	vector<int> neighboors, commonEdges;
	GetNeighboorVertices(inpG, inpV, neighboors);
	for (int i = 0; i < neighboors.size(); i++) {
		GetCommonEdges(inpG.Vertices[neighboors[i]], inpG.Vertices[inpV], commonEdges);
		for (int k = 0; k < commonEdges.size(); k++) { // Для каждой смежной вершины удаляем из её СИ номер ребра, инцидентного удаляемой вершине
			inpG.Vertices[neighboors[i]].IncidenceList.erase(inpG.Vertices[neighboors[i]].IncidenceList.begin()+GetEdgeNumber(inpG.Vertices[neighboors[i]],commonEdges[k]));
		}
		commonEdges.clear();
	}
#ifdef DEBUG
	cout << "Deleted vertex " << inpG.Vertices[inpV].name << "\n";
#endif
	inpG.Vertices.erase(inpG.Vertices.begin() + inpV);
}

// Удаление ребра из графа (по имени ребра)
void DeleteEdge(graph& inpG, int inpE) {
	for (int i = 0; i < inpG.Vertices.size(); i++) {
		for (int k = 0; k < inpG.Vertices[i].IncidenceList.size(); k++) {
			if (inpG.Vertices[i].IncidenceList[k] == inpE) {
				inpG.Vertices[i].IncidenceList.erase(inpG.Vertices[i].IncidenceList.begin() + k);
#ifdef DEBUG2
				cout << "deleted edge number " << k + 1 << " from vertex " << inpG.Vertices[i].name << "\n";
#endif 
			}
		}
	}
}

// Очистка СИ вершин от ребёр, не ведущих ни в одну вершину
void CleanIncidenceList(graph& inpG, int inpV) {
	vector<int> neighboors;
	GetNeighboorVertices(inpG, inpV, neighboors);
	if (neighboors.size() == 0) {
		inpG.Vertices[inpV].IncidenceList.clear();
		return;
	}

	vector<int> nonOddEdges;
	for (int i = 0; i < inpG.Vertices[inpV].IncidenceList.size(); i++) {
		for (int k = 0; k < neighboors.size(); k++) {
			if (count(inpG.Vertices[neighboors[k]].IncidenceList.begin(), inpG.Vertices[neighboors[k]].IncidenceList.end(), inpG.Vertices[inpV].IncidenceList[i])) {
				nonOddEdges.push_back(inpG.Vertices[inpV].IncidenceList[i]);
			}
		}
	}
	inpG.Vertices[inpV].IncidenceList=nonOddEdges;
}

void CleanAllIncidenceList(graph& inpG) {
	for (int i = 0; i < inpG.Vertices.size(); i++)	CleanIncidenceList(inpG, i);
#ifdef DEBUG2
	cout << "Cleaned incidence lists for " << inpG.Vertices.size() << " vertices.\n\n";
#endif
}

// Создание матрицы смежности данного графа
void CreateAdjacanceMatrix(graph& inpG, vector<vector<int>>& matrix) {
	matrix.clear();
	for (int i = 0; i < inpG.Vertices.size(); i++) { // Заполнение МС нулями
		matrix.push_back({});
		for (int k = 0; k < inpG.Vertices.size(); k++)	matrix[i].push_back(0);
	}

	for (int i = 0; i < inpG.Vertices.size(); i++) { // Заполнение единиц в МС
		vector<int> neighboors;
		GetNeighboorVertices(inpG, i, neighboors);
		for (int k = 0; k < neighboors.size(); k++) {
			matrix[i][neighboors[k]] = 1;
		}
	}

	for (int i = 0; i < inpG.Vertices.size(); i++) cout << inpG.Vertices[i].name << "\t"; // Вывод на экран матрицы
	cout << "\n";
	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix[i].size(); j++) {
			cout << matrix[i][j] << "\t";
		}
		cout << inpG.Vertices[i].name << "\n";
	}
}

// Поиск подграфа, изоморфного К5 (Вернее максимального подграфа, где содержатся несколько таковых)
graph GetSubgraph_K5(graph& inpG) { 
#ifdef DEBUG
	cout << "Trying to find K5-isomorphic subgraph.\n";
#endif

	vector<int> CandidatesLevel1; // 1 уровень кандидатов (по степени вершины >3)
	for (int i = 0; i < inpG.Vertices.size(); i++) {
		if (inpG.Vertices[i].degree() > 3) {
			CandidatesLevel1.push_back(i);
#ifdef DEBUG_K5
			cout << "Vertex " << inpG.Vertices[i].name << "\t-> 1 level candidates.\n";
#endif
		}
	}
#ifdef DEBUG_K5
	cout << "\n";
#endif
	if (CandidatesLevel1.size() < 5) { // Возвращаем пустой подграф если кандидатов 1 уровня < 5
#ifdef DEBUG
		cout << "No K5-isomorphic subgraph found.\n\n";
#endif
		return {};
	}

	graph Output; // 2 уровень кандидатов (по смежности с 4 вершинами-кандидитами 1 уровня), сразу пишутся в аутпут

	for (int i = 0; i < CandidatesLevel1.size(); i++) {
		int NeighboorCandidatesLevel1 = 0;

		for (int k = 0; k < CandidatesLevel1.size(); k++) {
			if(Neighboors(inpG, i, k) && i!=k) NeighboorCandidatesLevel1++;
		}
		if (NeighboorCandidatesLevel1 > 3) {
#ifdef DEBUG_K5
			cout << "Vertex " << inpG.Vertices[CandidatesLevel1[i]].name << "\t-> Output.\n";
#endif
			Output.Vertices.push_back(inpG.Vertices[CandidatesLevel1[i]]);
		}
	}
#ifdef DEBUG_K5
	cout << "\n";
#endif
	if (Output.Vertices.size() < 5) { // Возвращаем пустой подграф если кандидатов 2 уровня < 5
#ifdef DEBUG
		cout << "No K5-isomorphic subgraph found.\n\n";
#endif
		return {};
	}

	CleanAllIncidenceList(Output);
#ifdef DEBUG
	cout << "Succesfully returned K5-isomorphic subgraph.\n\n";
#endif
	return Output;
}

// Поиск подграфа, изоморфного К3,3 (Вернее максимального подграфа, где содержатся несколько таковых)
graph GetSubgraph_K33(graph& inpG) {
	graph CandidateGraph1; // Подграф из кандидатов 1 уровня
	for (int i = 0; i < inpG.Vertices.size(); i++) {
		if (inpG.Vertices[i].degree() > 2) CandidateGraph1.Vertices.push_back(inpG.Vertices[i]);
	}
	CleanAllIncidenceList(CandidateGraph1);

	for (int i = 0; i < CandidateGraph1.Vertices.size(); i++) { // После очистки СИ в графе кандидатов-1 вновь проверяем степени вершин, удаляем не подходящие
		if (inpG.Vertices[i].degree() < 3) {
			DeleteVertex(CandidateGraph1, i);
			i--; // Откатываем счетчик т.к. удаляем вершину
		}
	}
	if (CandidateGraph1.Vertices.size() < 6) return {}; // Возвращаем пустой подграф если кандидатов-1 < 6

	graph Output;

	unsigned int iteration = 1;
	do {
		cout << "\nNow on iteration " << iteration << "\n";
		graph IsomorphismTest;
		vector<vector<int>> IsomorphismTestMatrix;
		for (int i=0; i < 6; i++) IsomorphismTest.Vertices.push_back(CandidateGraph1.Vertices[i]); // Собираем проверяемый граф
		CleanAllIncidenceList(IsomorphismTest);
		IsomorphismTest.DisplayAllIncidenceList();
		bool NonK33 = false;
		for (int i = 0; i < IsomorphismTest.Vertices.size(); i++) {
			if (IsomorphismTest.Vertices[i].degree() < 3) {
				NonK33 = true;
				cout << "Exit from cycle on " << IsomorphismTest.Vertices[i].name <<  "\n";
				break;
			}
		}
		if (NonK33) {
			rotate(CandidateGraph1.Vertices.begin(), CandidateGraph1.Vertices.begin() + 1, CandidateGraph1.Vertices.end()); // Перемещаем вершины в массиве чтобы на следующей итерации получить новый проверяемый граф
			iteration++;
			continue; // Идем на следующую проверку если в получившемся графе степени вершин меньше чем надо
		}

		CreateAdjacanceMatrix(IsomorphismTest, IsomorphismTestMatrix);



		rotate(CandidateGraph1.Vertices.begin(), CandidateGraph1.Vertices.begin() + 1, CandidateGraph1.Vertices.end()); // Перемещаем вершины в массиве чтобы на следующей итерации получить новый проверяемый граф
		iteration++;
	} while (iteration<CandidateGraph1.Vertices.size()-5+2); // Выход когда проверили все перестановки


	//---Собираем граф из 6 вершин-кандидатов-1
	//---Дополнительные условия для оптимизации
	//---Генерируем МИ данного графа
	//Проверяем совпадение МИ графа-образца и сгенерированной МИ при всех возможных перестановках строк
	//Слепить в аутпут все найденные изоморфизмы


	if (Output.empty()) {
		cout << "No K3,3-isomorphic subgraph found.\n\n";
	} else cout << "Succesfully returned K3,3-isomorphic subgraph.\n\n";
	return Output;
}

// Проверка является ли граф планарным
bool Planar(graph& inpG) {
	if (3 * inpG.Vertices.size() - inpG.edgesCount() < 6) return false; // Следствие формулы Эйлера (3В-Р>=6 для планарных связных графов)
	if (GetSubgraph_K5(inpG).empty() && GetSubgraph_K33(inpG).empty()) return true; else return false;
}

// Удаление ребёр для превращения графа в планарный.
void MakePlanar(graph& input) {
	cout << "Trying to make graph planar\n";
	graph inpG = input; // Создаём граф внутри функции, из которого будем исключать вершины и делать иные вещи для поиска ответа. Оригинальный же затронем только удалением рёбер.
	ExcludeAllVertices(inpG);
	graph Subgraph_K5 = GetSubgraph_K5(inpG);
	graph Subgraph_K33 = GetSubgraph_K33(inpG);

	if (Subgraph_K5.empty() && Subgraph_K33.empty()) {
		cout << "Graph is already planar\n\n";
		return;
	}

#ifdef DEBUG
	cout << "Trying find common edges for two non-planar subgraphs\n";
#endif
	vector<int> potentialAnswer; // Массив рёбер, которые будем пробовать удалить. 
	GetCommonEdges(Subgraph_K5, Subgraph_K33, potentialAnswer); // В первую очередь - общие для двух непланарных подграфов рёбра.
	for (int i=0; i < Subgraph_K5.Vertices.size(); i++) { // Далее просто добавляем туда все рёбра непланарных подграфов
		for (int k=0; k < Subgraph_K5.Vertices[i].IncidenceList.size(); k++) {
			potentialAnswer.push_back(Subgraph_K5.Vertices[i].IncidenceList[k]);
		}
	}
	for (int i=0; i < Subgraph_K33.Vertices.size(); i++) {
		for (int k=0; k < Subgraph_K33.Vertices[i].IncidenceList.size(); k++) {
			potentialAnswer.push_back(Subgraph_K33.Vertices[i].IncidenceList[k]);
		}
	}
	CleanVector(potentialAnswer);

#ifdef DEBUG
	cout << "Potential edges list: ";
	for (int i = 0; i < potentialAnswer.size(); i++) cout << potentialAnswer[i] << " ";
	cout << "\n\n";
#endif
		int iteration = 0;
		int maxiteration = potentialAnswer.size();
		vector<int> needtodelete; // Массив рёбер, удаление которых необходимо для планарности
		graph t_Subgraph_K5; 
		graph t_Subgraph_K33; // Временные графы, используемые для проверки планарности после удаления рёбер в циклах

		do {
#ifdef DEBUG
			cout << "Now on iteration " << iteration + 1 << " (max iterations " << maxiteration << ")\n\n";
#endif
			for (int i = 0; i < needtodelete.size(); i++) { // Удаление вышеупомянутых рёбер из подграфов
				DeleteEdge(Subgraph_K5, needtodelete[i]);
				DeleteEdge(Subgraph_K33, needtodelete[i]);
			}

			bool isReadyK5 = true, isReadyK33 = true; // Проверка планарны ли графы после удаления рёбер

		if (!Subgraph_K5.empty()) {
			for (int i = 0; i < potentialAnswer.size(); i++) { // В первом цикле пробуем устранить К5
				t_Subgraph_K5 = Subgraph_K5;
#ifdef DEBUG
				cout << "Deleted edge " << potentialAnswer[i] << " (trying to make K5 planar)\n";
#endif
				DeleteEdge(t_Subgraph_K5, potentialAnswer[i]);
				if (GetSubgraph_K5(t_Subgraph_K5).empty()) {
					isReadyK5 = true;
#ifdef DEBUG
					cout << "Made K5 planar\n";
					cout << "Added edge " << potentialAnswer[i] << " to the answer\n\n";
#endif
					needtodelete.push_back(potentialAnswer[i]);
					DeleteEdge(input, potentialAnswer[i]);
					break;
				}
				else {
					isReadyK5 = false;
#ifdef DEBUG
					cout << "Did not make K5 planar\n";
#endif
				}
			}
		}

		if (!Subgraph_K33.empty()) {
			for (int i = 0; i < potentialAnswer.size(); i++) { // Во втором цикле пробуем устранить К3,3
				t_Subgraph_K33 = Subgraph_K33;
#ifdef DEBUG
				cout << "Deleted edge " << potentialAnswer[i] << " (trying to make K33 planar)\n";
#endif
				DeleteEdge(t_Subgraph_K33, potentialAnswer[i]);
				if (GetSubgraph_K33(t_Subgraph_K33).empty()) {
					isReadyK33 = true;
#ifdef DEBUG
					cout << "Made K33 planar\n";
					cout << "Added edge " << potentialAnswer[i] << " to the answer\n\n";
#endif
					needtodelete.push_back(potentialAnswer[i]);
					DeleteEdge(input, potentialAnswer[i]);
					break;
				}
				isReadyK33 = false;
#ifdef DEBUG
				cout << "Did not make K33 planar\n";
#endif
			}
		}

			if (isReadyK5 && isReadyK33) {
#ifdef DEBUG
				cout << "Made graph planar\n\n";
#endif
				break;
			}
#ifdef DEBUG
			cout << "Graph in not planar yet\n";
			cout << "Added " << potentialAnswer[0] << " to the answer\n";
#endif

			needtodelete.push_back(potentialAnswer[0]);
			DeleteEdge(input, potentialAnswer[0]);
			potentialAnswer.erase(potentialAnswer.begin());
			CleanVector(needtodelete);
			iteration++;
		} while (iteration<=maxiteration); // Принудительно завершаем проверку если проверили все рёбра и не помогло

		//CleanVector(needtodelete);
		cout << "Minimal list of edges to be deleted: ";
		for (int i = 0; i < needtodelete.size(); i++) {
			cout << needtodelete[i] << " ";
		}
		cout << "(" << needtodelete.size() << " edges)\n\n";
}

// MAIN

int main() {
	do {
		graph inp;
		char filename[] = { "test_input4.txt" };
		cout << "\n--- Pls input example number ---\n\n";

		filename[10] = _getch();
		inp = GetGraphFromFile(filename);
		
		graph k33=GetSubgraph_K33(inp);
		WriteGraphToFile(k33, "test_output.txt");

		cout << "\n--- Press q to exit ---\n\n";
	} while (_getch() != 'q');
	return 0;
}