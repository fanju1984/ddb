// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/ads/googleads/v1/resources/shared_criterion.proto

package resources

import (
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
	wrappers "github.com/golang/protobuf/ptypes/wrappers"
	common "google.golang.org/genproto/googleapis/ads/googleads/v1/common"
	enums "google.golang.org/genproto/googleapis/ads/googleads/v1/enums"
	_ "google.golang.org/genproto/googleapis/api/annotations"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

// A criterion belonging to a shared set.
type SharedCriterion struct {
	// Immutable. The resource name of the shared criterion.
	// Shared set resource names have the form:
	//
	// `customers/{customer_id}/sharedCriteria/{shared_set_id}~{criterion_id}`
	ResourceName string `protobuf:"bytes,1,opt,name=resource_name,json=resourceName,proto3" json:"resource_name,omitempty"`
	// Immutable. The shared set to which the shared criterion belongs.
	SharedSet *wrappers.StringValue `protobuf:"bytes,2,opt,name=shared_set,json=sharedSet,proto3" json:"shared_set,omitempty"`
	// Output only. The ID of the criterion.
	//
	// This field is ignored for mutates.
	CriterionId *wrappers.Int64Value `protobuf:"bytes,26,opt,name=criterion_id,json=criterionId,proto3" json:"criterion_id,omitempty"`
	// Output only. The type of the criterion.
	Type enums.CriterionTypeEnum_CriterionType `protobuf:"varint,4,opt,name=type,proto3,enum=google.ads.googleads.v1.enums.CriterionTypeEnum_CriterionType" json:"type,omitempty"`
	// The criterion.
	//
	// Exactly one must be set.
	//
	// Types that are valid to be assigned to Criterion:
	//	*SharedCriterion_Keyword
	//	*SharedCriterion_YoutubeVideo
	//	*SharedCriterion_YoutubeChannel
	//	*SharedCriterion_Placement
	//	*SharedCriterion_MobileAppCategory
	//	*SharedCriterion_MobileApplication
	Criterion            isSharedCriterion_Criterion `protobuf_oneof:"criterion"`
	XXX_NoUnkeyedLiteral struct{}                    `json:"-"`
	XXX_unrecognized     []byte                      `json:"-"`
	XXX_sizecache        int32                       `json:"-"`
}

func (m *SharedCriterion) Reset()         { *m = SharedCriterion{} }
func (m *SharedCriterion) String() string { return proto.CompactTextString(m) }
func (*SharedCriterion) ProtoMessage()    {}
func (*SharedCriterion) Descriptor() ([]byte, []int) {
	return fileDescriptor_c8be09b18890bd33, []int{0}
}

func (m *SharedCriterion) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_SharedCriterion.Unmarshal(m, b)
}
func (m *SharedCriterion) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_SharedCriterion.Marshal(b, m, deterministic)
}
func (m *SharedCriterion) XXX_Merge(src proto.Message) {
	xxx_messageInfo_SharedCriterion.Merge(m, src)
}
func (m *SharedCriterion) XXX_Size() int {
	return xxx_messageInfo_SharedCriterion.Size(m)
}
func (m *SharedCriterion) XXX_DiscardUnknown() {
	xxx_messageInfo_SharedCriterion.DiscardUnknown(m)
}

var xxx_messageInfo_SharedCriterion proto.InternalMessageInfo

func (m *SharedCriterion) GetResourceName() string {
	if m != nil {
		return m.ResourceName
	}
	return ""
}

func (m *SharedCriterion) GetSharedSet() *wrappers.StringValue {
	if m != nil {
		return m.SharedSet
	}
	return nil
}

func (m *SharedCriterion) GetCriterionId() *wrappers.Int64Value {
	if m != nil {
		return m.CriterionId
	}
	return nil
}

func (m *SharedCriterion) GetType() enums.CriterionTypeEnum_CriterionType {
	if m != nil {
		return m.Type
	}
	return enums.CriterionTypeEnum_UNSPECIFIED
}

type isSharedCriterion_Criterion interface {
	isSharedCriterion_Criterion()
}

type SharedCriterion_Keyword struct {
	Keyword *common.KeywordInfo `protobuf:"bytes,3,opt,name=keyword,proto3,oneof"`
}

type SharedCriterion_YoutubeVideo struct {
	YoutubeVideo *common.YouTubeVideoInfo `protobuf:"bytes,5,opt,name=youtube_video,json=youtubeVideo,proto3,oneof"`
}

type SharedCriterion_YoutubeChannel struct {
	YoutubeChannel *common.YouTubeChannelInfo `protobuf:"bytes,6,opt,name=youtube_channel,json=youtubeChannel,proto3,oneof"`
}

type SharedCriterion_Placement struct {
	Placement *common.PlacementInfo `protobuf:"bytes,7,opt,name=placement,proto3,oneof"`
}

type SharedCriterion_MobileAppCategory struct {
	MobileAppCategory *common.MobileAppCategoryInfo `protobuf:"bytes,8,opt,name=mobile_app_category,json=mobileAppCategory,proto3,oneof"`
}

type SharedCriterion_MobileApplication struct {
	MobileApplication *common.MobileApplicationInfo `protobuf:"bytes,9,opt,name=mobile_application,json=mobileApplication,proto3,oneof"`
}

func (*SharedCriterion_Keyword) isSharedCriterion_Criterion() {}

func (*SharedCriterion_YoutubeVideo) isSharedCriterion_Criterion() {}

func (*SharedCriterion_YoutubeChannel) isSharedCriterion_Criterion() {}

func (*SharedCriterion_Placement) isSharedCriterion_Criterion() {}

func (*SharedCriterion_MobileAppCategory) isSharedCriterion_Criterion() {}

func (*SharedCriterion_MobileApplication) isSharedCriterion_Criterion() {}

func (m *SharedCriterion) GetCriterion() isSharedCriterion_Criterion {
	if m != nil {
		return m.Criterion
	}
	return nil
}

func (m *SharedCriterion) GetKeyword() *common.KeywordInfo {
	if x, ok := m.GetCriterion().(*SharedCriterion_Keyword); ok {
		return x.Keyword
	}
	return nil
}

func (m *SharedCriterion) GetYoutubeVideo() *common.YouTubeVideoInfo {
	if x, ok := m.GetCriterion().(*SharedCriterion_YoutubeVideo); ok {
		return x.YoutubeVideo
	}
	return nil
}

func (m *SharedCriterion) GetYoutubeChannel() *common.YouTubeChannelInfo {
	if x, ok := m.GetCriterion().(*SharedCriterion_YoutubeChannel); ok {
		return x.YoutubeChannel
	}
	return nil
}

func (m *SharedCriterion) GetPlacement() *common.PlacementInfo {
	if x, ok := m.GetCriterion().(*SharedCriterion_Placement); ok {
		return x.Placement
	}
	return nil
}

func (m *SharedCriterion) GetMobileAppCategory() *common.MobileAppCategoryInfo {
	if x, ok := m.GetCriterion().(*SharedCriterion_MobileAppCategory); ok {
		return x.MobileAppCategory
	}
	return nil
}

func (m *SharedCriterion) GetMobileApplication() *common.MobileApplicationInfo {
	if x, ok := m.GetCriterion().(*SharedCriterion_MobileApplication); ok {
		return x.MobileApplication
	}
	return nil
}

// XXX_OneofWrappers is for the internal use of the proto package.
func (*SharedCriterion) XXX_OneofWrappers() []interface{} {
	return []interface{}{
		(*SharedCriterion_Keyword)(nil),
		(*SharedCriterion_YoutubeVideo)(nil),
		(*SharedCriterion_YoutubeChannel)(nil),
		(*SharedCriterion_Placement)(nil),
		(*SharedCriterion_MobileAppCategory)(nil),
		(*SharedCriterion_MobileApplication)(nil),
	}
}

func init() {
	proto.RegisterType((*SharedCriterion)(nil), "google.ads.googleads.v1.resources.SharedCriterion")
}

func init() {
	proto.RegisterFile("google/ads/googleads/v1/resources/shared_criterion.proto", fileDescriptor_c8be09b18890bd33)
}

var fileDescriptor_c8be09b18890bd33 = []byte{
	// 688 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x94, 0x94, 0xdb, 0x6a, 0xdb, 0x4a,
	0x14, 0x86, 0xb7, 0xed, 0x9c, 0x3c, 0x39, 0xb1, 0xb5, 0xf7, 0x85, 0x76, 0x76, 0x68, 0x93, 0xd0,
	0x80, 0x49, 0xc9, 0x28, 0x76, 0xdb, 0x50, 0x54, 0x28, 0xc8, 0xa6, 0xa4, 0xe9, 0x89, 0xe0, 0xa4,
	0x2e, 0x2d, 0x06, 0x31, 0x92, 0x56, 0x14, 0x11, 0x69, 0x46, 0x68, 0x24, 0x07, 0x13, 0x72, 0xd1,
	0x57, 0xc9, 0x65, 0x1f, 0xa5, 0x4f, 0x91, 0xeb, 0x3c, 0x42, 0xaf, 0x8a, 0x47, 0x33, 0xb2, 0xeb,
	0xd6, 0x75, 0x7a, 0xb7, 0xa4, 0xf5, 0xff, 0xdf, 0xbf, 0x34, 0xa3, 0x19, 0xf4, 0xd4, 0x67, 0xcc,
	0x0f, 0xc1, 0x20, 0x1e, 0x37, 0xf2, 0x72, 0x50, 0xf5, 0xea, 0x46, 0x02, 0x9c, 0x65, 0x89, 0x0b,
	0xdc, 0xe0, 0x67, 0x24, 0x01, 0xcf, 0x76, 0x93, 0x20, 0x85, 0x24, 0x60, 0x14, 0xc7, 0x09, 0x4b,
	0x99, 0xb6, 0x99, 0xcb, 0x31, 0xf1, 0x38, 0x2e, 0x9c, 0xb8, 0x57, 0xc7, 0x85, 0x73, 0x6d, 0x77,
	0x12, 0xdc, 0x65, 0x51, 0xc4, 0xa8, 0x21, 0x91, 0x24, 0x27, 0xae, 0x35, 0x26, 0xc9, 0x81, 0x66,
	0x11, 0x37, 0x8a, 0x01, 0xec, 0xb4, 0x1f, 0x83, 0xf4, 0xdc, 0x57, 0x9e, 0x38, 0x30, 0x4e, 0x03,
	0x08, 0x3d, 0xdb, 0x81, 0x33, 0xd2, 0x0b, 0x58, 0x22, 0x05, 0xff, 0x8d, 0x08, 0xd4, 0x64, 0xb2,
	0x75, 0x4f, 0xb6, 0xc4, 0x93, 0x93, 0x9d, 0x1a, 0x17, 0x09, 0x89, 0x63, 0x48, 0xb8, 0xec, 0xaf,
	0x8f, 0x58, 0x09, 0xa5, 0x2c, 0x25, 0x69, 0xc0, 0xa8, 0xec, 0x6e, 0x5d, 0x2f, 0xa0, 0xd5, 0x63,
	0xb1, 0x34, 0x2d, 0x35, 0x98, 0xf6, 0x1e, 0x2d, 0xab, 0x0c, 0x9b, 0x92, 0x08, 0xf4, 0xd2, 0x46,
	0xa9, 0x56, 0x6d, 0xee, 0xdd, 0x58, 0xb3, 0xdf, 0xac, 0x1d, 0x54, 0x1b, 0xae, 0x93, 0xac, 0xe2,
	0x80, 0x63, 0x97, 0x45, 0xc6, 0x18, 0xa8, 0xbd, 0xa4, 0x30, 0xef, 0x48, 0x04, 0x9a, 0x8f, 0x90,
	0xdc, 0x04, 0x0e, 0xa9, 0x5e, 0xde, 0x28, 0xd5, 0x16, 0x1b, 0xeb, 0x12, 0x81, 0xd5, 0xf4, 0xf8,
	0x38, 0x4d, 0x02, 0xea, 0x77, 0x48, 0x98, 0x41, 0x73, 0x47, 0x24, 0x3e, 0x40, 0x5b, 0x53, 0x12,
	0x8f, 0x21, 0x6d, 0x57, 0xb9, 0x2a, 0xb5, 0x16, 0x5a, 0x1a, 0xae, 0x72, 0xe0, 0xe9, 0x6b, 0x22,
	0xea, 0xff, 0x9f, 0xa2, 0x0e, 0x69, 0xba, 0xff, 0x38, 0x4f, 0xaa, 0xdc, 0x58, 0x95, 0xf6, 0x62,
	0xe1, 0x3a, 0xf4, 0xb4, 0x0f, 0x68, 0x66, 0xb0, 0x41, 0xfa, 0xcc, 0x46, 0xa9, 0xb6, 0xd2, 0x78,
	0x8e, 0x27, 0xfd, 0x27, 0x62, 0x57, 0x71, 0xf1, 0xcd, 0x27, 0xfd, 0x18, 0x5e, 0xd0, 0x2c, 0xfa,
	0xf1, 0x4d, 0xce, 0x17, 0x40, 0xed, 0x0d, 0x9a, 0x3f, 0x87, 0xfe, 0x05, 0x4b, 0x3c, 0xbd, 0x22,
	0x06, 0x7b, 0x38, 0x91, 0x9d, 0xff, 0x60, 0xf8, 0x75, 0x2e, 0x3f, 0xa4, 0xa7, 0x6c, 0x00, 0x9a,
	0x7d, 0xf9, 0x57, 0x5b, 0x21, 0xb4, 0x2e, 0x5a, 0xee, 0xb3, 0x2c, 0xcd, 0x1c, 0xb0, 0x7b, 0x81,
	0x07, 0x4c, 0x9f, 0x15, 0xcc, 0xbd, 0x69, 0xcc, 0x8f, 0x2c, 0x3b, 0xc9, 0x1c, 0xe8, 0x0c, 0x3c,
	0xa3, 0xe0, 0x25, 0x49, 0x13, 0x0d, 0xcd, 0x41, 0xab, 0x8a, 0xee, 0x9e, 0x11, 0x4a, 0x21, 0xd4,
	0xe7, 0x04, 0xbf, 0x71, 0x47, 0x7e, 0x2b, 0x77, 0x8d, 0x26, 0xac, 0x48, 0xa2, 0x6c, 0x69, 0x6d,
	0x54, 0x8d, 0x43, 0xe2, 0x42, 0x04, 0x34, 0xd5, 0xe7, 0x05, 0x7d, 0x77, 0x1a, 0xfd, 0x48, 0x19,
	0x46, 0xc1, 0x43, 0x8c, 0x16, 0xa2, 0x7f, 0x22, 0xe6, 0x04, 0x21, 0xd8, 0x24, 0x8e, 0x6d, 0x97,
	0xa4, 0xe0, 0xb3, 0xa4, 0xaf, 0x2f, 0x08, 0xfa, 0x93, 0x69, 0xf4, 0xb7, 0xc2, 0x6a, 0xc5, 0x71,
	0x4b, 0x1a, 0x47, 0x53, 0xfe, 0x8e, 0xc6, 0xbb, 0xda, 0x39, 0xd2, 0x86, 0x69, 0x61, 0xe0, 0x8a,
	0x03, 0xa6, 0x57, 0xff, 0x30, 0x4c, 0x19, 0x7f, 0x1d, 0xa6, 0xba, 0x26, 0xdc, 0x5a, 0xce, 0xdd,
	0x8f, 0xa0, 0xb6, 0xef, 0x66, 0x3c, 0x65, 0x11, 0x24, 0xdc, 0xb8, 0x54, 0xe5, 0x95, 0xbc, 0x0c,
	0xa5, 0x8a, 0x18, 0x97, 0xe3, 0x97, 0xe3, 0x55, 0x73, 0x11, 0x55, 0x8b, 0xa7, 0xe6, 0xe7, 0x32,
	0xda, 0x76, 0x59, 0x84, 0xa7, 0xde, 0x95, 0xcd, 0x7f, 0xc7, 0xf2, 0x8f, 0x06, 0x67, 0xed, 0xa8,
	0xf4, 0xe9, 0x95, 0xb4, 0xfa, 0x2c, 0x24, 0xd4, 0xc7, 0x2c, 0xf1, 0x0d, 0x1f, 0xa8, 0x38, 0x89,
	0xc6, 0xf0, 0x0b, 0x7e, 0x73, 0x7f, 0x3f, 0x2b, 0xaa, 0xeb, 0x72, 0xe5, 0xc0, 0xb2, 0xbe, 0x94,
	0x37, 0x0f, 0x72, 0xa4, 0xe5, 0x71, 0x9c, 0x97, 0x83, 0xaa, 0x53, 0xc7, 0x6d, 0xa5, 0xfc, 0xaa,
	0x34, 0x5d, 0xcb, 0xe3, 0xdd, 0x42, 0xd3, 0xed, 0xd4, 0xbb, 0x85, 0xe6, 0xb6, 0xbc, 0x9d, 0x37,
	0x4c, 0xd3, 0xf2, 0xb8, 0x69, 0x16, 0x2a, 0xd3, 0xec, 0xd4, 0x4d, 0xb3, 0xd0, 0x39, 0x73, 0x62,
	0xd8, 0x47, 0xdf, 0x03, 0x00, 0x00, 0xff, 0xff, 0x0d, 0x28, 0x47, 0x9c, 0x6b, 0x06, 0x00, 0x00,
}
