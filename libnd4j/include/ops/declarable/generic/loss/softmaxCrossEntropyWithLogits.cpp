/*******************************************************************************
 * Copyright (c) 2015-2018 Skymind, Inc.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License, Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

//
// @author Yurii Shyrma (iuriish@yahoo.com), created on 18.06.2018
//

#include <op_boilerplate.h>
#if NOT_EXCLUDED(OP_softmax_cross_entropy_loss_with_logits)

#include <ops/declarable/CustomOperations.h>

namespace nd4j {
namespace ops  {


//////////////////////////////////////////////////////////////////////////
CUSTOM_OP_IMPL(softmax_cross_entropy_loss_with_logits, 2, 1, false, 0, 0) {

  	NDArray<T>* logits  = INPUT_VARIABLE(0);
    NDArray<T>* labels  = INPUT_VARIABLE(1);
    NDArray<T>* output  = OUTPUT_VARIABLE(0);

    const int classesDim = block.getIArguments()->size() > 0 ? INT_ARG(0) : logits->rankOf()-1;
    
    // input validation    		       
    REQUIRE_TRUE(labels->isSameShape(logits), 0, "SOFTMAX_CROSS_ENTROPY_LOSS_WITH_LOGITS OP: labels and logits arrays must have the same shapes, but got %s and %s correspondingly !", ShapeUtils<T>::shapeAsString(labels).c_str(), ShapeUtils<T>::shapeAsString(logits).c_str());    
    REQUIRE_TRUE(classesDim < logits->rankOf(), 0, "SOFTMAX_CROSS_ENTROPY_LOSS_WITH_LOGITS OP: class dimension must be smaller than rank of logits, but got %i and %i correspondingly !", classesDim, logits->rankOf());
	
	T extraParams[1] = {static_cast<T>(classesDim)};
    NDArray<T> logExp = logits->template transform<simdOps::Exp<T>>();
    NDArray<T> logSoftMax = ( logExp / logExp.template reduceAlongDims<simdOps::Sum<T>>({classesDim}, true) ).template transform<simdOps::Log<T>>();
    
	output->assign( -((*labels) * logSoftMax).template reduceAlongDims<simdOps::Sum<T>>({classesDim}) );
   		
    return Status::OK();
}


DECLARE_SHAPE_FN(softmax_cross_entropy_loss_with_logits) {
	
	auto logitsShapeInfo  = inputShape->at(0);
    auto labelsShapeInfo  = inputShape->at(1);

    const int classesDim = block.getIArguments()->size() > 0 ? INT_ARG(0) : -1;
    std::vector<int> dimensions = {classesDim};

	// labels and logits must have the same shapes 
    REQUIRE_TRUE(shape::shapeEquals(logitsShapeInfo, labelsShapeInfo), 0, "SOFTMAX_CROSS_ENTROPY_LOSS_WITH_LOGITS OP: labels and logits arrays must have the same shapes, but got %s and %s correspondingly!", ShapeUtils<T>::shapeAsString(labelsShapeInfo).c_str(), ShapeUtils<T>::shapeAsString(logitsShapeInfo).c_str());    

    auto reducedShapeInfo = ShapeUtils<T>::evalReduceShapeInfo(shape::order(labelsShapeInfo), dimensions, labelsShapeInfo, false, false, block.getWorkspace());
   
    return SHAPELIST(reducedShapeInfo);    

}


}
}

#endif