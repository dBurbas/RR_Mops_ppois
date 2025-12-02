#pragma once

#include <sc-memory/sc_agent.hpp>

class CreateGraphAgent : public ScActionInitiatedAgent
{
public:
  // Конструктор агента: настраивает логгер и внутреннее состояние
  CreateGraphAgent();

  // Возвращение действия, для которого будет запускаться этот агент
  ScAddr GetActionClass() const override;

  // Находит или создаёт узел района по заданному текстовому имени
  void GetDistrict(ScAddr & districtNode, std::string const & nameOfDistrict);

  // Возвращает ScAddr элемента с заданным основным идентификатором (или ScAddr() если не найден)
  ScAddr GetElementByMainIdentifier(std::string const & district);

  // Помечает маршрут соответствующим видом транспорта (автобус, трамвай, метро)
  void GetTypeOfRoute(ScAddr & typeOfRoute, std::string const & nameOfRoute, ScStructure const & route);

  // Создаёт маршруты и дороги между районами по данным строк из csv и добавляет их в структуру города
  void GenerateRoutes(std::vector<std::string> const & resOfSplit, ScStructure & city);

  // Возвращает первый свободный номер для системного идентификатора с заданным базовым именем
  int GetNumberOfCurrentSystemIdentifier(std::string const & baseName);

  // Основная логика агента: читает путь к csv и строит транспортный граф города
  ScResult DoProgram(ScActionInitiatedEvent const & event, ScAction & action) override;
};