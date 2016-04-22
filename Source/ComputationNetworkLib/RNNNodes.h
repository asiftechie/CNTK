//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include "Basics.h"
#include "ComputationNode.h"
#include "Matrix.h"
#include "TensorView.h"

#include <unordered_set>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <list>
#include <memory>
#include <algorithm>
#include <utility>
#include <assert.h>

namespace Microsoft { namespace MSR { namespace CNTK {

// -----------------------------------------------------------------------
// RNNNode
// -----------------------------------------------------------------------

template <class ElemType>
class RNNNode : public ComputationNode<ElemType>, public NumInputs<2>
{
	typedef ComputationNode<ElemType> Base;
	UsingComputationNodeMembersBoilerplate;
	static const std::wstring TypeName() { return L"RNN"; }
	using Base::OperationName;                                                                                                                           \

public:
	RNNNode(DEVICEID_TYPE deviceId, const wstring& name);
	RNNNode(const ScriptableObjects::IConfigRecordPtr configp);

	void Save(File& fstream) const;
	virtual void Load(File& fstream, size_t modelVersion) override;

public:
	virtual void /*ComputationNode::*/ ForwardProp(const FrameRange& fr) override;
	virtual void /*ComputationNode::*/ BackpropTo(const size_t inputIndex, const FrameRange& fr) override;
	virtual void /*ComputationNodeBase::*/ Validate(bool isFinalValidationPass) override;

	// request matrices needed to do node function value evaluation
	virtual void RequestMatricesBeforeForwardProp(MatrixPool& matrixPool)
	{
		Base::RequestMatricesBeforeForwardProp(matrixPool);
		RequestMatrixFromPool(m_transposedInput, matrixPool);
		RequestMatrixFromPool(m_transposedOutput, matrixPool);
		RequestMatrixFromPool(m_transposedDInput, matrixPool);
		RequestMatrixFromPool(m_transposedDOutput, matrixPool);
	}

	// Is the output value of the computation node needed for computing
	virtual bool OutputUsedInComputingInputNodesGradients() const { return false; }

	// Is the output value of the specified  input node needed for computing
	virtual bool InputUsedInComputingInputNodesGradients(size_t /*childIndex*/) const { return false; }

protected:
	bool BackwardDataCalledYet;
	TensorShape shapeXT;
	TensorShape shapeYT;
	shared_ptr<Matrix<ElemType>> m_transposedInput;
	shared_ptr<Matrix<ElemType>> m_transposedOutput;
	shared_ptr<Matrix<ElemType>> m_transposedDInput;
	shared_ptr<Matrix<ElemType>> m_transposedDOutput;


private:
	TensorView<ElemType> TensorHelper(int inputIndex/*-1 for output*/, bool gradient/*instead of value*/, const FrameRange& fr);
	void TransposeHelper(const MatrixBasePtr matX, const TensorShape &shapeX, MatrixBasePtr matY, TensorShape &shapeY);

	size_t m_numLayers;
	size_t m_numHidden;

};

} } }