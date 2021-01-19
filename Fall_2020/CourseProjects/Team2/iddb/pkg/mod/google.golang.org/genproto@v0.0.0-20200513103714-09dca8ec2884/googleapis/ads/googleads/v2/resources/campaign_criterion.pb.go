// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/ads/googleads/v2/resources/campaign_criterion.proto

package resources

import (
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
	wrappers "github.com/golang/protobuf/ptypes/wrappers"
	common "google.golang.org/genproto/googleapis/ads/googleads/v2/common"
	enums "google.golang.org/genproto/googleapis/ads/googleads/v2/enums"
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

// A campaign criterion.
type CampaignCriterion struct {
	// Immutable. The resource name of the campaign criterion.
	// Campaign criterion resource names have the form:
	//
	// `customers/{customer_id}/campaignCriteria/{campaign_id}~{criterion_id}`
	ResourceName string `protobuf:"bytes,1,opt,name=resource_name,json=resourceName,proto3" json:"resource_name,omitempty"`
	// Immutable. The campaign to which the criterion belongs.
	Campaign *wrappers.StringValue `protobuf:"bytes,4,opt,name=campaign,proto3" json:"campaign,omitempty"`
	// Output only. The ID of the criterion.
	//
	// This field is ignored during mutate.
	CriterionId *wrappers.Int64Value `protobuf:"bytes,5,opt,name=criterion_id,json=criterionId,proto3" json:"criterion_id,omitempty"`
	// The modifier for the bids when the criterion matches. The modifier must be
	// in the range: 0.1 - 10.0. Most targetable criteria types support modifiers.
	// Use 0 to opt out of a Device type.
	BidModifier *wrappers.FloatValue `protobuf:"bytes,14,opt,name=bid_modifier,json=bidModifier,proto3" json:"bid_modifier,omitempty"`
	// Immutable. Whether to target (`false`) or exclude (`true`) the criterion.
	Negative *wrappers.BoolValue `protobuf:"bytes,7,opt,name=negative,proto3" json:"negative,omitempty"`
	// Output only. The type of the criterion.
	Type enums.CriterionTypeEnum_CriterionType `protobuf:"varint,6,opt,name=type,proto3,enum=google.ads.googleads.v2.enums.CriterionTypeEnum_CriterionType" json:"type,omitempty"`
	// The status of the criterion.
	Status enums.CampaignCriterionStatusEnum_CampaignCriterionStatus `protobuf:"varint,35,opt,name=status,proto3,enum=google.ads.googleads.v2.enums.CampaignCriterionStatusEnum_CampaignCriterionStatus" json:"status,omitempty"`
	// The campaign criterion.
	//
	// Exactly one must be set.
	//
	// Types that are valid to be assigned to Criterion:
	//	*CampaignCriterion_Keyword
	//	*CampaignCriterion_Placement
	//	*CampaignCriterion_MobileAppCategory
	//	*CampaignCriterion_MobileApplication
	//	*CampaignCriterion_Location
	//	*CampaignCriterion_Device
	//	*CampaignCriterion_AdSchedule
	//	*CampaignCriterion_AgeRange
	//	*CampaignCriterion_Gender
	//	*CampaignCriterion_IncomeRange
	//	*CampaignCriterion_ParentalStatus
	//	*CampaignCriterion_UserList
	//	*CampaignCriterion_YoutubeVideo
	//	*CampaignCriterion_YoutubeChannel
	//	*CampaignCriterion_Proximity
	//	*CampaignCriterion_Topic
	//	*CampaignCriterion_ListingScope
	//	*CampaignCriterion_Language
	//	*CampaignCriterion_IpBlock
	//	*CampaignCriterion_ContentLabel
	//	*CampaignCriterion_Carrier
	//	*CampaignCriterion_UserInterest
	//	*CampaignCriterion_Webpage
	//	*CampaignCriterion_OperatingSystemVersion
	//	*CampaignCriterion_MobileDevice
	//	*CampaignCriterion_LocationGroup
	//	*CampaignCriterion_CustomAffinity
	Criterion            isCampaignCriterion_Criterion `protobuf_oneof:"criterion"`
	XXX_NoUnkeyedLiteral struct{}                      `json:"-"`
	XXX_unrecognized     []byte                        `json:"-"`
	XXX_sizecache        int32                         `json:"-"`
}

func (m *CampaignCriterion) Reset()         { *m = CampaignCriterion{} }
func (m *CampaignCriterion) String() string { return proto.CompactTextString(m) }
func (*CampaignCriterion) ProtoMessage()    {}
func (*CampaignCriterion) Descriptor() ([]byte, []int) {
	return fileDescriptor_395bdd66d6311d9f, []int{0}
}

func (m *CampaignCriterion) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_CampaignCriterion.Unmarshal(m, b)
}
func (m *CampaignCriterion) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_CampaignCriterion.Marshal(b, m, deterministic)
}
func (m *CampaignCriterion) XXX_Merge(src proto.Message) {
	xxx_messageInfo_CampaignCriterion.Merge(m, src)
}
func (m *CampaignCriterion) XXX_Size() int {
	return xxx_messageInfo_CampaignCriterion.Size(m)
}
func (m *CampaignCriterion) XXX_DiscardUnknown() {
	xxx_messageInfo_CampaignCriterion.DiscardUnknown(m)
}

var xxx_messageInfo_CampaignCriterion proto.InternalMessageInfo

func (m *CampaignCriterion) GetResourceName() string {
	if m != nil {
		return m.ResourceName
	}
	return ""
}

func (m *CampaignCriterion) GetCampaign() *wrappers.StringValue {
	if m != nil {
		return m.Campaign
	}
	return nil
}

func (m *CampaignCriterion) GetCriterionId() *wrappers.Int64Value {
	if m != nil {
		return m.CriterionId
	}
	return nil
}

func (m *CampaignCriterion) GetBidModifier() *wrappers.FloatValue {
	if m != nil {
		return m.BidModifier
	}
	return nil
}

func (m *CampaignCriterion) GetNegative() *wrappers.BoolValue {
	if m != nil {
		return m.Negative
	}
	return nil
}

func (m *CampaignCriterion) GetType() enums.CriterionTypeEnum_CriterionType {
	if m != nil {
		return m.Type
	}
	return enums.CriterionTypeEnum_UNSPECIFIED
}

func (m *CampaignCriterion) GetStatus() enums.CampaignCriterionStatusEnum_CampaignCriterionStatus {
	if m != nil {
		return m.Status
	}
	return enums.CampaignCriterionStatusEnum_UNSPECIFIED
}

type isCampaignCriterion_Criterion interface {
	isCampaignCriterion_Criterion()
}

type CampaignCriterion_Keyword struct {
	Keyword *common.KeywordInfo `protobuf:"bytes,8,opt,name=keyword,proto3,oneof"`
}

type CampaignCriterion_Placement struct {
	Placement *common.PlacementInfo `protobuf:"bytes,9,opt,name=placement,proto3,oneof"`
}

type CampaignCriterion_MobileAppCategory struct {
	MobileAppCategory *common.MobileAppCategoryInfo `protobuf:"bytes,10,opt,name=mobile_app_category,json=mobileAppCategory,proto3,oneof"`
}

type CampaignCriterion_MobileApplication struct {
	MobileApplication *common.MobileApplicationInfo `protobuf:"bytes,11,opt,name=mobile_application,json=mobileApplication,proto3,oneof"`
}

type CampaignCriterion_Location struct {
	Location *common.LocationInfo `protobuf:"bytes,12,opt,name=location,proto3,oneof"`
}

type CampaignCriterion_Device struct {
	Device *common.DeviceInfo `protobuf:"bytes,13,opt,name=device,proto3,oneof"`
}

type CampaignCriterion_AdSchedule struct {
	AdSchedule *common.AdScheduleInfo `protobuf:"bytes,15,opt,name=ad_schedule,json=adSchedule,proto3,oneof"`
}

type CampaignCriterion_AgeRange struct {
	AgeRange *common.AgeRangeInfo `protobuf:"bytes,16,opt,name=age_range,json=ageRange,proto3,oneof"`
}

type CampaignCriterion_Gender struct {
	Gender *common.GenderInfo `protobuf:"bytes,17,opt,name=gender,proto3,oneof"`
}

type CampaignCriterion_IncomeRange struct {
	IncomeRange *common.IncomeRangeInfo `protobuf:"bytes,18,opt,name=income_range,json=incomeRange,proto3,oneof"`
}

type CampaignCriterion_ParentalStatus struct {
	ParentalStatus *common.ParentalStatusInfo `protobuf:"bytes,19,opt,name=parental_status,json=parentalStatus,proto3,oneof"`
}

type CampaignCriterion_UserList struct {
	UserList *common.UserListInfo `protobuf:"bytes,22,opt,name=user_list,json=userList,proto3,oneof"`
}

type CampaignCriterion_YoutubeVideo struct {
	YoutubeVideo *common.YouTubeVideoInfo `protobuf:"bytes,20,opt,name=youtube_video,json=youtubeVideo,proto3,oneof"`
}

type CampaignCriterion_YoutubeChannel struct {
	YoutubeChannel *common.YouTubeChannelInfo `protobuf:"bytes,21,opt,name=youtube_channel,json=youtubeChannel,proto3,oneof"`
}

type CampaignCriterion_Proximity struct {
	Proximity *common.ProximityInfo `protobuf:"bytes,23,opt,name=proximity,proto3,oneof"`
}

type CampaignCriterion_Topic struct {
	Topic *common.TopicInfo `protobuf:"bytes,24,opt,name=topic,proto3,oneof"`
}

type CampaignCriterion_ListingScope struct {
	ListingScope *common.ListingScopeInfo `protobuf:"bytes,25,opt,name=listing_scope,json=listingScope,proto3,oneof"`
}

type CampaignCriterion_Language struct {
	Language *common.LanguageInfo `protobuf:"bytes,26,opt,name=language,proto3,oneof"`
}

type CampaignCriterion_IpBlock struct {
	IpBlock *common.IpBlockInfo `protobuf:"bytes,27,opt,name=ip_block,json=ipBlock,proto3,oneof"`
}

type CampaignCriterion_ContentLabel struct {
	ContentLabel *common.ContentLabelInfo `protobuf:"bytes,28,opt,name=content_label,json=contentLabel,proto3,oneof"`
}

type CampaignCriterion_Carrier struct {
	Carrier *common.CarrierInfo `protobuf:"bytes,29,opt,name=carrier,proto3,oneof"`
}

type CampaignCriterion_UserInterest struct {
	UserInterest *common.UserInterestInfo `protobuf:"bytes,30,opt,name=user_interest,json=userInterest,proto3,oneof"`
}

type CampaignCriterion_Webpage struct {
	Webpage *common.WebpageInfo `protobuf:"bytes,31,opt,name=webpage,proto3,oneof"`
}

type CampaignCriterion_OperatingSystemVersion struct {
	OperatingSystemVersion *common.OperatingSystemVersionInfo `protobuf:"bytes,32,opt,name=operating_system_version,json=operatingSystemVersion,proto3,oneof"`
}

type CampaignCriterion_MobileDevice struct {
	MobileDevice *common.MobileDeviceInfo `protobuf:"bytes,33,opt,name=mobile_device,json=mobileDevice,proto3,oneof"`
}

type CampaignCriterion_LocationGroup struct {
	LocationGroup *common.LocationGroupInfo `protobuf:"bytes,34,opt,name=location_group,json=locationGroup,proto3,oneof"`
}

type CampaignCriterion_CustomAffinity struct {
	CustomAffinity *common.CustomAffinityInfo `protobuf:"bytes,36,opt,name=custom_affinity,json=customAffinity,proto3,oneof"`
}

func (*CampaignCriterion_Keyword) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_Placement) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_MobileAppCategory) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_MobileApplication) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_Location) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_Device) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_AdSchedule) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_AgeRange) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_Gender) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_IncomeRange) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_ParentalStatus) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_UserList) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_YoutubeVideo) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_YoutubeChannel) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_Proximity) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_Topic) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_ListingScope) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_Language) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_IpBlock) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_ContentLabel) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_Carrier) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_UserInterest) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_Webpage) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_OperatingSystemVersion) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_MobileDevice) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_LocationGroup) isCampaignCriterion_Criterion() {}

func (*CampaignCriterion_CustomAffinity) isCampaignCriterion_Criterion() {}

func (m *CampaignCriterion) GetCriterion() isCampaignCriterion_Criterion {
	if m != nil {
		return m.Criterion
	}
	return nil
}

func (m *CampaignCriterion) GetKeyword() *common.KeywordInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Keyword); ok {
		return x.Keyword
	}
	return nil
}

func (m *CampaignCriterion) GetPlacement() *common.PlacementInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Placement); ok {
		return x.Placement
	}
	return nil
}

func (m *CampaignCriterion) GetMobileAppCategory() *common.MobileAppCategoryInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_MobileAppCategory); ok {
		return x.MobileAppCategory
	}
	return nil
}

func (m *CampaignCriterion) GetMobileApplication() *common.MobileApplicationInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_MobileApplication); ok {
		return x.MobileApplication
	}
	return nil
}

func (m *CampaignCriterion) GetLocation() *common.LocationInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Location); ok {
		return x.Location
	}
	return nil
}

func (m *CampaignCriterion) GetDevice() *common.DeviceInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Device); ok {
		return x.Device
	}
	return nil
}

func (m *CampaignCriterion) GetAdSchedule() *common.AdScheduleInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_AdSchedule); ok {
		return x.AdSchedule
	}
	return nil
}

func (m *CampaignCriterion) GetAgeRange() *common.AgeRangeInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_AgeRange); ok {
		return x.AgeRange
	}
	return nil
}

func (m *CampaignCriterion) GetGender() *common.GenderInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Gender); ok {
		return x.Gender
	}
	return nil
}

func (m *CampaignCriterion) GetIncomeRange() *common.IncomeRangeInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_IncomeRange); ok {
		return x.IncomeRange
	}
	return nil
}

func (m *CampaignCriterion) GetParentalStatus() *common.ParentalStatusInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_ParentalStatus); ok {
		return x.ParentalStatus
	}
	return nil
}

func (m *CampaignCriterion) GetUserList() *common.UserListInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_UserList); ok {
		return x.UserList
	}
	return nil
}

func (m *CampaignCriterion) GetYoutubeVideo() *common.YouTubeVideoInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_YoutubeVideo); ok {
		return x.YoutubeVideo
	}
	return nil
}

func (m *CampaignCriterion) GetYoutubeChannel() *common.YouTubeChannelInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_YoutubeChannel); ok {
		return x.YoutubeChannel
	}
	return nil
}

func (m *CampaignCriterion) GetProximity() *common.ProximityInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Proximity); ok {
		return x.Proximity
	}
	return nil
}

func (m *CampaignCriterion) GetTopic() *common.TopicInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Topic); ok {
		return x.Topic
	}
	return nil
}

func (m *CampaignCriterion) GetListingScope() *common.ListingScopeInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_ListingScope); ok {
		return x.ListingScope
	}
	return nil
}

func (m *CampaignCriterion) GetLanguage() *common.LanguageInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Language); ok {
		return x.Language
	}
	return nil
}

func (m *CampaignCriterion) GetIpBlock() *common.IpBlockInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_IpBlock); ok {
		return x.IpBlock
	}
	return nil
}

func (m *CampaignCriterion) GetContentLabel() *common.ContentLabelInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_ContentLabel); ok {
		return x.ContentLabel
	}
	return nil
}

func (m *CampaignCriterion) GetCarrier() *common.CarrierInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Carrier); ok {
		return x.Carrier
	}
	return nil
}

func (m *CampaignCriterion) GetUserInterest() *common.UserInterestInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_UserInterest); ok {
		return x.UserInterest
	}
	return nil
}

func (m *CampaignCriterion) GetWebpage() *common.WebpageInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_Webpage); ok {
		return x.Webpage
	}
	return nil
}

func (m *CampaignCriterion) GetOperatingSystemVersion() *common.OperatingSystemVersionInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_OperatingSystemVersion); ok {
		return x.OperatingSystemVersion
	}
	return nil
}

func (m *CampaignCriterion) GetMobileDevice() *common.MobileDeviceInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_MobileDevice); ok {
		return x.MobileDevice
	}
	return nil
}

func (m *CampaignCriterion) GetLocationGroup() *common.LocationGroupInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_LocationGroup); ok {
		return x.LocationGroup
	}
	return nil
}

func (m *CampaignCriterion) GetCustomAffinity() *common.CustomAffinityInfo {
	if x, ok := m.GetCriterion().(*CampaignCriterion_CustomAffinity); ok {
		return x.CustomAffinity
	}
	return nil
}

// XXX_OneofWrappers is for the internal use of the proto package.
func (*CampaignCriterion) XXX_OneofWrappers() []interface{} {
	return []interface{}{
		(*CampaignCriterion_Keyword)(nil),
		(*CampaignCriterion_Placement)(nil),
		(*CampaignCriterion_MobileAppCategory)(nil),
		(*CampaignCriterion_MobileApplication)(nil),
		(*CampaignCriterion_Location)(nil),
		(*CampaignCriterion_Device)(nil),
		(*CampaignCriterion_AdSchedule)(nil),
		(*CampaignCriterion_AgeRange)(nil),
		(*CampaignCriterion_Gender)(nil),
		(*CampaignCriterion_IncomeRange)(nil),
		(*CampaignCriterion_ParentalStatus)(nil),
		(*CampaignCriterion_UserList)(nil),
		(*CampaignCriterion_YoutubeVideo)(nil),
		(*CampaignCriterion_YoutubeChannel)(nil),
		(*CampaignCriterion_Proximity)(nil),
		(*CampaignCriterion_Topic)(nil),
		(*CampaignCriterion_ListingScope)(nil),
		(*CampaignCriterion_Language)(nil),
		(*CampaignCriterion_IpBlock)(nil),
		(*CampaignCriterion_ContentLabel)(nil),
		(*CampaignCriterion_Carrier)(nil),
		(*CampaignCriterion_UserInterest)(nil),
		(*CampaignCriterion_Webpage)(nil),
		(*CampaignCriterion_OperatingSystemVersion)(nil),
		(*CampaignCriterion_MobileDevice)(nil),
		(*CampaignCriterion_LocationGroup)(nil),
		(*CampaignCriterion_CustomAffinity)(nil),
	}
}

func init() {
	proto.RegisterType((*CampaignCriterion)(nil), "google.ads.googleads.v2.resources.CampaignCriterion")
}

func init() {
	proto.RegisterFile("google/ads/googleads/v2/resources/campaign_criterion.proto", fileDescriptor_395bdd66d6311d9f)
}

var fileDescriptor_395bdd66d6311d9f = []byte{
	// 1247 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x94, 0x97, 0xdd, 0x6e, 0xd4, 0x46,
	0x1b, 0xc7, 0xdf, 0x04, 0x12, 0x92, 0xd9, 0x4d, 0x78, 0x19, 0x5a, 0x3a, 0x04, 0x0a, 0x09, 0x14,
	0x09, 0x28, 0xd8, 0xed, 0xf6, 0xe3, 0x60, 0x51, 0x91, 0xbc, 0xdb, 0x92, 0x6e, 0x1b, 0x4a, 0xb4,
	0xa1, 0x8b, 0xa8, 0x22, 0x59, 0x63, 0x7b, 0xe2, 0x4c, 0x63, 0xcf, 0x58, 0x63, 0x7b, 0xe9, 0x2a,
	0xe2, 0xa8, 0x77, 0xd2, 0xc3, 0xde, 0x47, 0x4f, 0x7a, 0x15, 0x1c, 0x73, 0x09, 0x3d, 0xaa, 0xe6,
	0xc3, 0x8e, 0xcd, 0x26, 0xb5, 0x39, 0x9b, 0x9d, 0xe7, 0xf9, 0xff, 0x9e, 0x8f, 0xf5, 0xcc, 0x63,
	0x83, 0x7e, 0xc8, 0x79, 0x18, 0x11, 0x1b, 0x07, 0xa9, 0xad, 0x97, 0x72, 0x35, 0xed, 0xd9, 0x82,
	0xa4, 0x3c, 0x17, 0x3e, 0x49, 0x6d, 0x1f, 0xc7, 0x09, 0xa6, 0x21, 0x73, 0x7d, 0x41, 0x33, 0x22,
	0x28, 0x67, 0x56, 0x22, 0x78, 0xc6, 0xe1, 0x96, 0x16, 0x58, 0x38, 0x48, 0xad, 0x52, 0x6b, 0x4d,
	0x7b, 0x56, 0xa9, 0xdd, 0x78, 0x78, 0x16, 0xde, 0xe7, 0x71, 0xcc, 0x99, 0x6d, 0x90, 0x58, 0x13,
	0x37, 0xbe, 0x39, 0xcb, 0x9d, 0xb0, 0x3c, 0x3e, 0x2d, 0x13, 0x37, 0xcd, 0x70, 0x96, 0xa7, 0x46,
	0xde, 0x6b, 0x90, 0x97, 0xaa, 0x6c, 0x96, 0x10, 0xa3, 0xb9, 0x59, 0x68, 0x12, 0x6a, 0x1f, 0x50,
	0x12, 0x05, 0xae, 0x47, 0x0e, 0xf1, 0x94, 0x72, 0x61, 0x1c, 0xae, 0x56, 0x1c, 0x8a, 0xc2, 0x8c,
	0xe9, 0x86, 0x31, 0xa9, 0x5f, 0x5e, 0x7e, 0x60, 0xbf, 0x12, 0x38, 0x49, 0x88, 0x28, 0xf2, 0xb9,
	0x5e, 0x91, 0x62, 0xc6, 0x78, 0x86, 0x33, 0xca, 0x99, 0xb1, 0xde, 0xfa, 0x0b, 0x81, 0x4b, 0x43,
	0x53, 0xd1, 0xb0, 0x48, 0x0d, 0xbe, 0x00, 0x6b, 0x45, 0x14, 0x97, 0xe1, 0x98, 0xa0, 0x85, 0xcd,
	0x85, 0xbb, 0xab, 0x83, 0xde, 0x1b, 0x67, 0xe9, 0x1f, 0xe7, 0x01, 0xb8, 0x7f, 0xd2, 0x68, 0xb3,
	0x4a, 0x68, 0x6a, 0xf9, 0x3c, 0xb6, 0xe7, 0x50, 0xe3, 0x6e, 0x01, 0xfa, 0x09, 0xc7, 0x04, 0xfa,
	0x60, 0xa5, 0xe8, 0x1f, 0x3a, 0xbf, 0xb9, 0x70, 0xb7, 0xd3, 0xbb, 0x6e, 0x10, 0x56, 0x91, 0xbf,
	0xb5, 0x97, 0x09, 0xca, 0xc2, 0x09, 0x8e, 0x72, 0x32, 0xb8, 0xa7, 0x22, 0xde, 0x06, 0x5b, 0x8d,
	0x11, 0xc7, 0x25, 0x18, 0x0e, 0x41, 0xf7, 0xa4, 0xcb, 0x34, 0x40, 0x4b, 0x2a, 0xd0, 0xb5, 0xb9,
	0x40, 0x23, 0x96, 0x7d, 0xfd, 0xa5, 0x8e, 0x73, 0xee, 0x8d, 0x73, 0x6e, 0xdc, 0x29, 0x55, 0xa3,
	0x00, 0x3e, 0x06, 0x5d, 0x8f, 0x06, 0x6e, 0xcc, 0x03, 0x7a, 0x40, 0x89, 0x40, 0xeb, 0x67, 0x40,
	0x9e, 0x44, 0x1c, 0x67, 0x0a, 0x32, 0xee, 0x78, 0x34, 0x78, 0x6a, 0xfc, 0xe1, 0x23, 0xb0, 0xc2,
	0x48, 0x88, 0x33, 0x3a, 0x25, 0xe8, 0x82, 0xd2, 0x6e, 0xcc, 0x69, 0x07, 0x9c, 0x47, 0x65, 0xfc,
	0xa5, 0x71, 0x29, 0x80, 0x2f, 0xc0, 0x79, 0xf9, 0x74, 0xa0, 0xe5, 0xcd, 0x85, 0xbb, 0xeb, 0xbd,
	0xc7, 0xd6, 0x59, 0xcf, 0xb8, 0x7a, 0xa4, 0xac, 0xb2, 0xd9, 0xcf, 0x67, 0x09, 0xf9, 0x8e, 0xe5,
	0x71, 0x7d, 0x47, 0x17, 0xa7, 0x80, 0xf0, 0x57, 0xb0, 0xac, 0x1f, 0x56, 0x74, 0x5b, 0xa1, 0xc7,
	0x4d, 0xe8, 0x77, 0xff, 0xcf, 0x3d, 0xa5, 0xd6, 0x41, 0x4e, 0xb7, 0x8d, 0x4d, 0x04, 0xb8, 0x03,
	0x2e, 0x1c, 0x91, 0xd9, 0x2b, 0x2e, 0x02, 0xb4, 0xa2, 0x1a, 0xf0, 0xe9, 0x99, 0xc1, 0xf4, 0x41,
	0xb4, 0x7e, 0xd4, 0xee, 0x23, 0x76, 0xc0, 0x55, 0x47, 0xbe, 0xff, 0xdf, 0xb8, 0x40, 0xc0, 0x31,
	0x58, 0x4d, 0x22, 0xec, 0x93, 0x98, 0xb0, 0x0c, 0xad, 0x2a, 0xde, 0xc3, 0x26, 0xde, 0x6e, 0x21,
	0xa8, 0x12, 0x4f, 0x30, 0x30, 0x02, 0x97, 0x63, 0xee, 0xd1, 0x88, 0xb8, 0x38, 0x49, 0x5c, 0x1f,
	0x67, 0x24, 0xe4, 0x62, 0x86, 0x80, 0xa2, 0x7f, 0xd5, 0x44, 0x7f, 0xaa, 0xa4, 0x4e, 0x92, 0x0c,
	0x8d, 0xb0, 0x1a, 0xe5, 0x52, 0xfc, 0xae, 0x15, 0x1e, 0x01, 0x78, 0x12, 0x2d, 0xa2, 0xbe, 0x3a,
	0x87, 0xa8, 0xf3, 0x9e, 0xc1, 0x0a, 0xe1, 0xe9, 0xc1, 0x0a, 0x2b, 0x7c, 0x06, 0x56, 0x22, 0x6e,
	0x42, 0x74, 0x55, 0x88, 0x07, 0x4d, 0x21, 0x76, 0xf8, 0x3c, 0xb9, 0x84, 0xc0, 0x11, 0x58, 0x0e,
	0xc8, 0x94, 0xfa, 0x04, 0xad, 0x29, 0xdc, 0xfd, 0x26, 0xdc, 0xb7, 0xca, 0xbb, 0x0a, 0x33, 0x00,
	0x38, 0x01, 0x1d, 0x1c, 0xb8, 0xa9, 0x7f, 0x48, 0x82, 0x3c, 0x22, 0xe8, 0xa2, 0xe2, 0x59, 0x4d,
	0x3c, 0x27, 0xd8, 0x33, 0x8a, 0x2a, 0x13, 0xe0, 0x72, 0x1b, 0xee, 0x82, 0x55, 0x1c, 0x12, 0x57,
	0x60, 0x16, 0x12, 0xf4, 0xff, 0x76, 0x45, 0x3b, 0x21, 0x19, 0x4b, 0xff, 0x5a, 0xd1, 0xd8, 0x6c,
	0xca, 0xa2, 0x43, 0xc2, 0x02, 0x22, 0xd0, 0xa5, 0x76, 0x45, 0x6f, 0x2b, 0xef, 0x5a, 0xd1, 0x1a,
	0x00, 0x5f, 0x82, 0x2e, 0x65, 0x3e, 0x8f, 0x8b, 0xfc, 0xa0, 0x02, 0xda, 0x4d, 0xc0, 0x91, 0xd2,
	0xcc, 0xa5, 0xd8, 0xa1, 0x27, 0xfb, 0xd0, 0x03, 0x17, 0x13, 0x2c, 0x08, 0xcb, 0x70, 0x64, 0x46,
	0x11, 0xba, 0xac, 0xe8, 0xbd, 0xc6, 0x03, 0x62, 0x64, 0xfa, 0xe4, 0x56, 0x03, 0xac, 0x27, 0x35,
	0x93, 0xec, 0x6d, 0x9e, 0x12, 0xe1, 0x46, 0x34, 0xcd, 0xd0, 0x95, 0x76, 0xbd, 0xfd, 0x39, 0x25,
	0x62, 0x87, 0xa6, 0xb5, 0xd3, 0xb7, 0x92, 0x9b, 0x4d, 0xb8, 0x0f, 0xd6, 0x66, 0x3c, 0xcf, 0x72,
	0x8f, 0xb8, 0x53, 0x1a, 0x10, 0x8e, 0x3e, 0x50, 0xd4, 0xcf, 0x9a, 0xa8, 0x2f, 0x79, 0xfe, 0x3c,
	0xf7, 0xc8, 0x44, 0x6a, 0xaa, 0xe4, 0xae, 0xa1, 0x29, 0x83, 0xec, 0x49, 0x41, 0xf7, 0x0f, 0x31,
	0x63, 0x24, 0x42, 0x1f, 0xb6, 0xeb, 0x89, 0xe1, 0x0f, 0xb5, 0xaa, 0xd6, 0x13, 0x43, 0x34, 0x26,
	0x75, 0x25, 0x09, 0xfe, 0x1b, 0x8d, 0x69, 0x36, 0x43, 0x1f, 0xb5, 0xbc, 0x92, 0x0a, 0x41, 0xfd,
	0x4a, 0x2a, 0x76, 0xe1, 0x13, 0xb0, 0x94, 0xf1, 0x84, 0xfa, 0x08, 0x29, 0xde, 0xbd, 0x26, 0xde,
	0x73, 0xe9, 0x5c, 0x65, 0x69, 0xb9, 0xec, 0xae, 0xfc, 0xab, 0x28, 0x0b, 0xdd, 0xd4, 0xe7, 0x09,
	0x41, 0x57, 0xdb, 0x75, 0x77, 0x47, 0x8b, 0xf6, 0xa4, 0xa6, 0xd6, 0xdd, 0xa8, 0x62, 0x50, 0xb7,
	0x0b, 0x66, 0x61, 0x8e, 0x43, 0x82, 0x36, 0x5a, 0xde, 0x2e, 0xc6, 0xbf, 0x7e, 0xbb, 0x98, 0x4d,
	0xf8, 0x14, 0xac, 0xd0, 0xc4, 0xf5, 0x22, 0xee, 0x1f, 0xa1, 0x6b, 0xed, 0x86, 0xc5, 0x28, 0x19,
	0x48, 0xf7, 0xda, 0xb0, 0xa0, 0x7a, 0x4f, 0x56, 0xef, 0x73, 0x96, 0x11, 0x96, 0xb9, 0x11, 0xf6,
	0x48, 0x84, 0xae, 0xb7, 0xab, 0x7e, 0xa8, 0x45, 0x3b, 0x52, 0x53, 0xab, 0xde, 0xaf, 0x18, 0xe4,
	0x60, 0xf3, 0xb1, 0x10, 0xf2, 0xad, 0xe0, 0xe3, 0x76, 0xb9, 0x0e, 0xb5, 0x7b, 0x2d, 0x57, 0x83,
	0x90, 0xb9, 0xaa, 0x93, 0x45, 0x59, 0x46, 0x04, 0x49, 0x33, 0x74, 0xa3, 0x5d, 0xae, 0xf2, 0x74,
	0x8d, 0x8c, 0xa6, 0x96, 0x6b, 0x5e, 0x31, 0xc8, 0x5c, 0x5f, 0x11, 0x2f, 0x91, 0x7f, 0xd4, 0xcd,
	0x76, 0xb9, 0xbe, 0xd0, 0xee, 0xb5, 0x5c, 0x0d, 0x02, 0x1e, 0x03, 0xc4, 0x13, 0x22, 0xb0, 0x7e,
	0xae, 0x66, 0x69, 0x46, 0x62, 0x77, 0x4a, 0x44, 0x2a, 0xa7, 0xcc, 0xa6, 0xc2, 0xf7, 0x9b, 0xf0,
	0xcf, 0x0a, 0xfd, 0x9e, 0x92, 0x4f, 0xb4, 0xba, 0x1a, 0xed, 0x0a, 0x3f, 0xd5, 0x45, 0x36, 0xca,
	0xcc, 0x4f, 0x33, 0x88, 0xb6, 0xda, 0x35, 0x4a, 0x8f, 0xce, 0xf9, 0x71, 0xd4, 0x8d, 0x2b, 0x06,
	0xe8, 0x82, 0xf5, 0x62, 0xd6, 0xb9, 0xa1, 0xe0, 0x79, 0x82, 0x6e, 0x29, 0xfc, 0xe7, 0x6d, 0xc7,
	0xe6, 0xb6, 0x14, 0x55, 0xf9, 0x6b, 0x51, 0xd5, 0x22, 0x6f, 0x24, 0x3f, 0x4f, 0x33, 0x1e, 0xbb,
	0xf8, 0xe0, 0x80, 0x32, 0x79, 0x67, 0x7c, 0xd2, 0xee, 0x46, 0x1a, 0x2a, 0x99, 0x63, 0x54, 0xb5,
	0x1b, 0xc9, 0xaf, 0x99, 0xfa, 0x47, 0x6f, 0x9d, 0xc3, 0xf7, 0x79, 0x3b, 0x87, 0x7d, 0x0d, 0x20,
	0x22, 0xb5, 0x8f, 0x8b, 0xe5, 0xeb, 0xf2, 0x13, 0xc7, 0xf8, 0x61, 0xfb, 0x78, 0xfe, 0xa3, 0xe7,
	0xf5, 0xa0, 0x03, 0x56, 0xcb, 0x5f, 0x83, 0xdf, 0x17, 0xc1, 0x1d, 0x9f, 0xc7, 0x56, 0xe3, 0xd7,
	0xd8, 0xe0, 0xca, 0x5c, 0x16, 0xbb, 0xf2, 0x7d, 0x78, 0x77, 0xe1, 0x97, 0x1f, 0x8c, 0x38, 0xe4,
	0xf2, 0x5e, 0xb0, 0xb8, 0x08, 0xed, 0x90, 0x30, 0xf5, 0xb6, 0x6c, 0x9f, 0x54, 0xf2, 0x1f, 0x5f,
	0x89, 0x8f, 0xca, 0xd5, 0x1f, 0x8b, 0xe7, 0xb6, 0x1d, 0xe7, 0xcf, 0xc5, 0xad, 0x6d, 0x8d, 0x74,
	0x82, 0xd4, 0xd2, 0x4b, 0xb9, 0x9a, 0xf4, 0xac, 0x71, 0xe1, 0xf9, 0x77, 0xe1, 0xb3, 0xef, 0x04,
	0xe9, 0x7e, 0xe9, 0xb3, 0x3f, 0xe9, 0xed, 0x97, 0x3e, 0x6f, 0x17, 0xef, 0x68, 0x43, 0xbf, 0xef,
	0x04, 0x69, 0xbf, 0x5f, 0x7a, 0xf5, 0xfb, 0x93, 0x5e, 0xbf, 0x5f, 0xfa, 0x79, 0xcb, 0x2a, 0xd9,
	0x2f, 0xfe, 0x0d, 0x00, 0x00, 0xff, 0xff, 0x54, 0xa7, 0xbc, 0x71, 0xd1, 0x0e, 0x00, 0x00,
}
