
#include <Bug.hxx>
#include <MoveAction.hxx>
#include <EatAction.hxx>
#include <GrowAction.hxx>
#include <Statistics.hxx>
#include <World.hxx>

namespace Examples
{

Bug::Bug( const std::string & id, const int &maxConsumptionRate) : Agent(id),  _maxConsumptionRate(maxConsumptionRate){
	 _size = 1; 
	 _color = "pink"; 
 }

Bug::~Bug()
{
}

void Bug::selectActions()
{
	_actions.push_back(new MoveAction());
	_actions.push_back(new EatAction());
	_actions.push_back(new GrowAction());
}

void Bug::updateState()
{
}

void Bug::registerAttributes()
{
	registerStringAttribute("color");
	registerIntAttribute("size");
}

void Bug::serialize()
{
	serializeAttribute("color", _color);
	serializeAttribute("size", _size);
}

void Bug::setSize(const int &size) {
	_size = size;
}

int Bug::getSize() const {
	return _size;
}

void Bug::setColor(const int &code) {
	if (code == 0) this->_color = "white";
	else {
		switch (code/10){
			
			case 0:
				this->_color = "pink";
				break;
			
			case 1:
				this->_color = "pink";
				break;
			
			case 2: 
				this->_color = "pink1";
				break;
			
			case 3: 
				this->_color = "pink2";
				break;
			
			case 4: 
				this->_color = "palevioletred1";
				break;
			
			case 5: 
				this->_color = "palevioletred2";
				break;
			
			case 6: 
				this->_color = "indianred2"; 
				break;
			
			case 7:
				this->_color = "indianred3";
				break;
			
			case 8: 
				this->_color = "red3";
				break;
			
			case 9: 
				this->_color = "indianred1";
				break;
			
			default: 
				this->_color = "red";
				break;
		}
	}
}

std::string Bug::getColor() const {
	return _color;
}

int Bug::getMaxConsumptionRate() const {
	return _maxConsumptionRate;
}

} // namespace Examples

