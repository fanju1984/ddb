package SQL

/*每一个table都有一个HF（水平划分）信息
根据select剪枝得HF_select
根据join剪枝得HF_join
两个取交集就得最后水平划分table
*/
type HF struct {
	pos    int
	tab    string
	hf_seg []string
	size   int
}

type HF_join struct {
	pos      int
	tab      string
	hf_seg_j []string
	size     int
}

type HF_select struct {
	pos      int
	tab      string
	hf_seg_s []string
	size     int
}

func NewHF(pos int, tab string, hf_seg []string) HF {
	return HF{
		pos:    pos,
		tab:    tab,
		hf_seg: hf_seg,
		size:   len(hf_seg),
	}
}

func NewHF_join(pos int, tab string, hf_seg_j []string) HF_join {
	return HF_join{
		pos:      pos,
		tab:      tab,
		hf_seg_j: hf_seg_j,
		size:     len(hf_seg_j),
	}
}

func NewHF_select(pos int, tab string, hf_seg_s []string) HF_select {
	return HF_select{
		pos:      pos,
		tab:      tab,
		hf_seg_s: hf_seg_s,
		size:     len(hf_seg_s),
	}
}
