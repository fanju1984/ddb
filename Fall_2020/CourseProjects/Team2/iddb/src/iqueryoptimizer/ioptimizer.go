package iqueryoptimizer

import (
	"iplan"
)

func Optimize(oldtree iplan.PlanTree) iplan.PlanTree {
	loopmax := 1
	var newtree iplan.PlanTree
	newtree = oldtree
	for i := 0; i < loopmax; i++ {
		newtree = GetRelCols(newtree)
		// newtree = JointConditionPushDown(newtree)
		newtree = SelectionPushDown(newtree)
		newtree = ProjectionPushDown(newtree)
		newtree = TransmissionMinimization(newtree)
		newtree = GetRelCols(newtree)
	}
	return newtree
}
