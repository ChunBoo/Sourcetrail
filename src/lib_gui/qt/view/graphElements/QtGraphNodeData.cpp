#include "qt/view/graphElements/QtGraphNodeData.h"

#include "utility/file/FilePath.h"
#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageDeactivateEdge.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageTooltipShow.h"
#include "utility/ResourcePaths.h"

#include "data/graph/token_component/TokenComponentFilePath.h"

QtGraphNodeData::QtGraphNodeData(const Node* data, const std::wstring& name, bool childVisible, bool hasQualifier, bool isInteractive)
	: m_data(data)
	, m_childVisible(childVisible)
	, m_hasQualifier(hasQualifier)
	, m_isInteractive(isInteractive)
{
	this->setAcceptHoverEvents(true);
	this->setName(name);
}

QtGraphNodeData::~QtGraphNodeData()
{
}

const Node* QtGraphNodeData::getData() const
{
	return m_data;
}

FilePath QtGraphNodeData::getFilePath() const
{
	TokenComponentFilePath* component = m_data->getComponent<TokenComponentFilePath>();
	if (component)
	{
		return component->getFilePath();
	}

	return FilePath();
}

bool QtGraphNodeData::isDataNode() const
{
	return true;
}

Id QtGraphNodeData::getTokenId() const
{
	return m_data->getId();
}

void QtGraphNodeData::onClick()
{
	if (m_isActive && !m_multipleActive)
	{
		MessageDeactivateEdge(true).dispatch();
		return;
	}

	MessageActivateNodes(m_data->getId()).dispatch();
}

void QtGraphNodeData::updateStyle()
{
	GraphViewStyle::NodeStyle style = GraphViewStyle::getStyleForNodeType(
		m_data->getType(), m_data->isExplicit(), m_isActive, m_isHovering, m_childVisible, m_hasQualifier);

	TokenComponentFilePath* component = m_data->getComponent<TokenComponentFilePath>();
	if (component && !component->isComplete())
	{
		style.iconPath = ResourcePaths::getGuiPath().concatenate(L"graph_view/images/file_incomplete.png");
	}

	setStyle(style);
}

void QtGraphNodeData::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	MessageFocusIn(std::vector<Id>(1, m_data->getId()), TOOLTIP_ORIGIN_GRAPH).dispatch();

	if (!m_isInteractive)
	{
		TooltipInfo info;
		info.title = NodeType::getReadableTypeWString(m_data->getType().getType());
		info.offset = Vec2i(10, 20);

		if (!m_data->isDefined())
		{
			info.title = L"non-indexed " + info.title;
		}
		else if (m_data->isImplicit())
		{
			info.title = L"implicit " + info.title;
		}

		MessageTooltipShow msg(info, TOOLTIP_ORIGIN_GRAPH);
		msg.setSendAsTask(true);
		msg.dispatch();
	}
}

void QtGraphNodeData::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	MessageFocusOut(std::vector<Id>(1, m_data->getId())).dispatch();
}
