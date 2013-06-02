//
//  CommonUtils.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-3.
//
//

#ifndef __Projector__CommonUtils__
#define __Projector__CommonUtils__

#include "AE_GeneralPlug.h"

/// 根据 AEGP_MemHandle 创建一个 A_UTF16Char 字符串的副本, 注意: 必须 delete 返回的字符串
extern A_UTF16Char * UTF16CharDuplicate(AEGP_MemorySuite1 * memSuite, AEGP_MemHandle memHandle);

/// 把 A_Time 的 value 转化为以秒为单位的时间
extern float obtainDurationInSecond(A_Time time);

/// 把 A_Time 的 scale 转化为 FPS
extern float obtainFrameratePerSecond(A_Time time);

/// 获得 Layer 对应的资源大小, 如果没有当前没有选中 layer, 则  *pWidth, *pHeight 都为 0
extern void obtainSourceItemDimensions(A_long * pWidth, A_long * pHeight);

/// 获得 Composition 的 width 和 height
extern void obtainCompositionItemDimensions(A_long * pWidth, A_long * pHeight);

/// 获得初始化的 AnchorPoint 值, 以像素为单位, 以左下角为坐标原点
extern void obtainInitAnchorPointData(AEGP_TwoDVal * pValue);

/// 获得初始化的 AnchorPoint 值, 以百分比为单位
extern void obtainInitAnchorPointDataWithPercent(AEGP_TwoDVal * pValue);

/// 获得初始化的 Position, 以点为单位
extern void obtainInitPositionData(AEGP_TwoDVal * pValue);

/// 获得初始化的 Scale, 以百分比为单位, 100% 原始大小
extern void obtainInitScaleData(AEGP_TwoDVal * pValue);

/// 获得初始化的 Rotation, 以度为单位
extern void obtainInitRotationData(AEGP_OneDVal * pValue);

/// 获得初始化的 Opacity, 以百分比为单位, 100% 不透明, 0% 全透明
extern void obtainInitOpacityData(AEGP_OneDVal * pValue);

/**
 * 获得 AnchorPoint 的动画数据, 以像素为单位, 以图片的左下角为原点
 * 注意: 使用完成后, 删除 *ppValues 和 *ppTimes 指向的内存数据
 * @param ppValue 存储的是 AnchorPoint 的实际坐标以像素为单位, 如果没有动画返回
 * @param ppTimes 存储的是相对时间, 以秒为单位
 * @param pNumkfs 帧的数量
 */
extern void obtainKeyframesAnchorPointData(AEGP_TwoDVal ** ppValues, float ** ppTimes, A_long * pNumkfs);

/**
 * 获得 AnchorPoint 的动画数据, 百分比为单位, 以图片的左下角为原点
 * 注意: 使用完成后, 删除 *ppValues 和 *ppTimes 指向的内存数据
 * @param ppValue 存储的是 AnchorPoint 的实际坐标以百分比为单位
 * @param ppTimes 存储的是相对时间, 以秒为单位
 * @param pNumkfs 帧的数量
 */
extern void obtainKeyframesAnchorPointDataWithPercent(AEGP_TwoDVal ** ppValues, float ** ppTimes, A_long * pNumkfs);

/// 获得 Position 的动画数据
extern void obtainKeyframesPositionData(AEGP_TwoDVal ** ppValues, float ** ppTimes, A_long * pNumkfs);

/// 获得 Scale 的动画数据
extern void obtainKeyframesScaleData(AEGP_TwoDVal ** ppValues, float ** ppTimes, A_long * pNumkfs);

/// 获得 Rotation 的动画数据
extern void obtainKeyframesRotationData(AEGP_OneDVal ** ppValues, float ** ppTimes, A_long * pNumkfs);

/// 获得 Opacity 的动画数据
extern void obtainKeyframesOpacityData(AEGP_OneDVal ** ppValues, float ** ppTimes, A_long * pNumkfs);

#endif /* defined(__Projector__CommonUtils__) */
