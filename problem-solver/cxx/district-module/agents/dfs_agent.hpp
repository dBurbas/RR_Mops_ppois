#pragma once

#include <sc-memory/sc_agent.hpp>

using ConnectivityEvent = ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc>;

// Агент анализирует транспортный граф города: связность, компоненты и мосты.
class TransportNetDFSAgent : public ScAgent<ConnectivityEvent>
{
public:
  // Конструктор агента: настраивает логгер и внутреннее состояние
  TransportNetDFSAgent();
  // Элемент, появление которого инициирует работу агента (готовность графа города к анализу)
  ScAddr GetEventSubscriptionElement() const override;
  // Шаблон условия запуска: любая дуга от action_ready_to_analyze_city_routes.
  ScTemplate GetInitiationConditionTemplate(ConnectivityEvent const & event) const override;
  // Возвращение действия, для которого будет запускаться этот агент
  ScAddr GetActionClass() const override;
  // Считает количество рёбер, исходящих из заданного района
  int GetCountOfEdges(ScAddr const & el);
  // Возвращает соседний район по индексу (итерация по рёбрам)
  ScAddr GetElementByIterator(ScAddr const & el, int index);
  // Собирает множество всех районов города по структурам маршрутов.
  ScAddrUnorderedSet GetDistricts(ScAddr const & city);
  //   std::string GetMainIndentifier(std::string const & district);
  // Выполняет обход в глубину от startDistrict и возвращает компоненту связности.
  ScAddrUnorderedSet FindConnection(
      ScAddrUnorderedSet & districts,
      ScAddr const & startDistrict,
      ScAddrUnorderedSet & visitedDistricts);
  // Находит мосты в графе (рёбра, увеличивающие количество компонент при удалении).
  void FindBridges(
      ScAddrUnorderedSet & districts,
      std::vector<std::pair<std::string, std::string>> & bridges,
      std::vector<std::pair<ScAddr, ScAddr>> & bridgesAddr);
  // Помечает граф как связный или несвязный в зависимости от числа компонент.
  void DefineTypeOfGraph(std::vector<ScAddrUnorderedSet> & resComponents, ScAddr & city);
  // Запускает DFS для всех ещё не посещённых районов и формирует список компонент.
  void GetResultOfConnectivity(
      std::vector<ScAddrUnorderedSet> & resComponents,
      ScAddrUnorderedSet & districts,
      ScAddrUnorderedSet & visitedDistricts);
  // Создаёт структуры компонент связности и связывает их с городом и nodeTuple.
  void GetComponents(std::vector<ScAddrUnorderedSet> & resComponents, ScAddr & city, ScAddr & nodeTuple);
  // Находит sc‑структуры, соответствующие мостам, и помечает их rrel_bridge.
  void GetBridges(std::vector<std::pair<ScAddr, ScAddr>> & bridgesAddr, ScAddr & nodeTuple, ScAddr & city);
  // Основная программа агента: запускает анализ связности, отмечает компоненты и мосты.
  ScResult DoProgram(ConnectivityEvent const & event, ScAction & action) override;
};