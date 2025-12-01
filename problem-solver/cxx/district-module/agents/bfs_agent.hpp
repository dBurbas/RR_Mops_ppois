#pragma once

#include <sc-memory/sc_agent.hpp>
#include <sc-memory/sc_agent.hpp>
#include <vector>

using TransportNetBFSEvent = ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>;

// Агент анализирует транспортную сеть города с помощью обхода в ширину: вычисляет кратчайшие пути, центральные районы,
// диаметр транспортной сети
class TransportNetBFSAgent : public ScAgent<TransportNetBFSEvent>
{
public:
  // Конструктор агента: настраивает логгер и внутреннее состояние
  TransportNetBFSAgent();
  // Элемент, появление которого инициирует работу агента (готовность графа города к анализу)
  ScAddr GetEventSubscriptionElement() const override;
  // Шаблон условия запуска: любая дуга от action_ready_to_analyze_city_routes.
  ScTemplate GetInitiationConditionTemplate(TransportNetBFSEvent const & event) const override;
  // Возвращение действия, для которого будет запускаться этот агент
  ScAddr GetActionClass() const override;
  // Основная программа агента: вычисляет диаметр,кратчайшие пути и определяет центральные районы в городе
  ScResult DoProgram(TransportNetBFSEvent const & event, ScAction & action) override;

  // Считает диаметр транспортной сети и находит центральные районы.
  void CalculateCentralRegionAndDiameterBFS(ScAddr & city);
  // Помечает районы с эксцентриситетом == radius как центральные.
  void FindCentralDistricts(ScAddr & city, std::vector<int> const & eccentrics, int const radius);
  // Одноисточниковый BFS от района с номером start, заполняет массив dist.
  void BFSShortestPathsSingleSource(int start, std::vector<int> & dist, int numberOfDistricts) const;
  // Возвращает контейнер(хэш-таблица) всех районов города, найденных по структурам маршрутов.
  ScAddrUnorderedSet GetDistricts(ScAddr const & city);
  // Находит кратчайшие расстояния от startVertex до всех районов.
  void FindShortestWays(
      ScAddrUnorderedSet const & districts,
      ScAddr const & startVertex,
      std::map<std::string, int> & resDists);
  // Строит структуры всех кратчайших путей между районами в городе.
  void SearchShortestWaysInCity(ScAddrUnorderedSet const & districts, ScAddr & city);
};