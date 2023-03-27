#pragma once

#define ACTION_LAYER_TIME		0
#define ACTION_LAYER_MOTION		1
#define ACTION_LAYER_EYE		2
#define ACTION_LAYER_EAR		3
#define ACTION_LAYER_MUSIC		4

class IUILayerControl
{
public:
	virtual bool Connect(QWidget* pParent) = 0;
	virtual void ReleaseConnection() = 0;
	virtual int GetType() = 0;
	virtual QWidget* GetQWidget() = 0;
	virtual void LayerDebugStop() = 0;
	virtual void EnablePlay(bool bState) = 0;
};

bool CreateUILayerControl(IUILayerControl** ppUILayerControl);