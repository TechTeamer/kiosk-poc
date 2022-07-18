#ifndef PR22_COUNTRYCODE_INCL
#define PR22_COUNTRYCODE_INCL
/******************************************************************************/
/**	\file
 *	Translates country codes to country names.
 */
/******************************************************************************/
#define PR22_VERSION 2.2.0.19
#include "Util/Types.hpp"
#include <map>
#include <string>
//------------------------------------------------------------------------------
namespace Pr22 { namespace Extension {

/// Translates country codes to country names.
class CountryCode {
public:

	/** Returns the name of the country.
	 *
	 * @param code		Country code.
	 * @return			Country name.
	 */
	static PrOutputString GetName(const std::wstring & code)
	{
		std::map<std::wstring, std::wstring> &countries_code = Codes();
		if(countries_code.empty()) Fill();
		return ToPrOutputString(countries_code[code]);
	}

private:

	static void Fill(void) {

		std::map<std::wstring, std::wstring> &countries_code = Codes();
		if(!countries_code.empty()) return;

		countries_code[L"AFG"] = L"Afghanistan";
		countries_code[L"ALA"] = L"\xC5land Islands";		//Åland Islands
		countries_code[L"ALB"] = L"Albania";
		countries_code[L"DZA"] = L"Algeria";
		countries_code[L"ASM"] = L"American Samoa";
		countries_code[L"AND"] = L"Andorra";
		countries_code[L"AGO"] = L"Angola";
		countries_code[L"AIA"] = L"Anguilla";
		countries_code[L"ATA"] = L"Antarctica";
		countries_code[L"ATG"] = L"Antigua and Barbuda";
		countries_code[L"ARG"] = L"Argentina";
		countries_code[L"ARM"] = L"Armenia";
		countries_code[L"ABW"] = L"Aruba";
		countries_code[L"AUS"] = L"Australia";
		countries_code[L"AUT"] = L"Austria";
		countries_code[L"AZE"] = L"Azerbaijan";
		countries_code[L"BHS"] = L"Bahamas";
		countries_code[L"BHR"] = L"Bahrain";
		countries_code[L"BGD"] = L"Bangladesh";
		countries_code[L"BRB"] = L"Barbados";
		countries_code[L"BLR"] = L"Belarus";
		countries_code[L"BEL"] = L"Belgium";
		countries_code[L"BLZ"] = L"Belize";
		countries_code[L"BEN"] = L"Benin";
		countries_code[L"BMU"] = L"Bermuda";
		countries_code[L"BTN"] = L"Bhutan";
		countries_code[L"BOL"] = L"Bolivia";
		countries_code[L"BES"] = L"Bonaire, Sint Eustatius and Saba";
		countries_code[L"BIH"] = L"Bosnia and Herzegovina";
		countries_code[L"BWA"] = L"Botswana";
		countries_code[L"BVT"] = L"Bouvet Island";
		countries_code[L"BRA"] = L"Brazil";
		countries_code[L"IOT"] = L"British Indian Ocean Territory";
		countries_code[L"BRN"] = L"Brunei Darussalam";
		countries_code[L"BGR"] = L"Bulgaria";
		countries_code[L"BFA"] = L"Burkina Faso";
		countries_code[L"BDI"] = L"Burundi";
		countries_code[L"CPV"] = L"Cabo Verde";
		countries_code[L"KHM"] = L"Cambodia";
		countries_code[L"CMR"] = L"Cameroon";
		countries_code[L"CAN"] = L"Canada";
		countries_code[L"CYM"] = L"Cayman Islands";
		countries_code[L"CAF"] = L"Central African Republic";
		countries_code[L"TCD"] = L"Chad";
		countries_code[L"CHL"] = L"Chile";
		countries_code[L"CHN"] = L"China";
		countries_code[L"CXR"] = L"Christmas Island";
		countries_code[L"CCK"] = L"Cocos (Keeling) Islands";
		countries_code[L"COL"] = L"Colombia";
		countries_code[L"COM"] = L"Comoros";
		countries_code[L"COD"] = L"Democratic Republic of the Congo";
		countries_code[L"COG"] = L"Congo";
		countries_code[L"COK"] = L"Cook Islands";
		countries_code[L"CRI"] = L"Costa Rica";
		countries_code[L"CIV"] = L"C\xF4te d'Ivoire";		//Côte d'Ivoire
		countries_code[L"HRV"] = L"Croatia";
		countries_code[L"CUB"] = L"Cuba";
		countries_code[L"CUW"] = L"Cura\xE7\x61o";			//Curaçao
		countries_code[L"CYP"] = L"Cyprus";
		countries_code[L"CZE"] = L"Czechia";
		countries_code[L"DNK"] = L"Denmark";
		countries_code[L"DJI"] = L"Djibouti";
		countries_code[L"DMA"] = L"Dominica";
		countries_code[L"DOM"] = L"Dominican Republic";
		countries_code[L"ECU"] = L"Ecuador";
		countries_code[L"EGY"] = L"Egypt";
		countries_code[L"SLV"] = L"El Salvador";
		countries_code[L"GNQ"] = L"Equatorial Guinea";
		countries_code[L"ERI"] = L"Eritrea";
		countries_code[L"EST"] = L"Estonia";
		countries_code[L"SWZ"] = L"Eswatini";
		countries_code[L"ETH"] = L"Ethiopia";
		countries_code[L"FLK"] = L"Falkland Islands (Malvinas)";
		countries_code[L"FRO"] = L"Faroe Islands";
		countries_code[L"FJI"] = L"Fiji";
		countries_code[L"FIN"] = L"Finland";
		countries_code[L"FRA"] = L"France";
		countries_code[L"GUF"] = L"French Guiana";
		countries_code[L"PYF"] = L"French Polynesia";
		countries_code[L"ATF"] = L"French Southern Territories";
		countries_code[L"GAB"] = L"Gabon";
		countries_code[L"GMB"] = L"Gambia";
		countries_code[L"GEO"] = L"Georgia";
		countries_code[L"D"] =   L"Germany";
		countries_code[L"GHA"] = L"Ghana";
		countries_code[L"GIB"] = L"Gibraltar";
		countries_code[L"GRC"] = L"Greece";
		countries_code[L"GRL"] = L"Greenland";
		countries_code[L"GRD"] = L"Grenada";
		countries_code[L"GLP"] = L"Guadeloupe";
		countries_code[L"GUM"] = L"Guam";
		countries_code[L"GTM"] = L"Guatemala";
		countries_code[L"GGY"] = L"Guernsey";
		countries_code[L"GIN"] = L"Guinea";
		countries_code[L"GNB"] = L"Guinea-Bissau";
		countries_code[L"GUY"] = L"Guyana";
		countries_code[L"HTI"] = L"Haiti";
		countries_code[L"HMD"] = L"Heard and McDonald Islands";
		countries_code[L"VAT"] = L"Holy See (Vatican City State)";
		countries_code[L"HND"] = L"Honduras";
		countries_code[L"HKG"] = L"Hong Kong";
		countries_code[L"HUN"] = L"Hungary";
		countries_code[L"ISL"] = L"Iceland";
		countries_code[L"IND"] = L"India";
		countries_code[L"IDN"] = L"Indonesia";
		countries_code[L"IRN"] = L"Islamic Republic of Iran";
		countries_code[L"IRQ"] = L"Iraq";
		countries_code[L"IRL"] = L"Ireland";
		countries_code[L"IMN"] = L"Isle of Man";
		countries_code[L"ISR"] = L"Israel";
		countries_code[L"ITA"] = L"Italy";
		countries_code[L"JAM"] = L"Jamaica";
		countries_code[L"JPN"] = L"Japan";
		countries_code[L"JEY"] = L"Jersey";
		countries_code[L"JOR"] = L"Jordan";
		countries_code[L"KAZ"] = L"Kazakhstan";
		countries_code[L"KEN"] = L"Kenya";
		countries_code[L"KIR"] = L"Kiribati";
		countries_code[L"PRK"] = L"Democratic People's Republic of Korea";
		countries_code[L"KOR"] = L"Republic of Korea";
		countries_code[L"KWT"] = L"Kuwait";
		countries_code[L"KGZ"] = L"Kyrgyzstan";
		countries_code[L"LAO"] = L"Lao People's Democratic Republic";
		countries_code[L"LVA"] = L"Latvia";
		countries_code[L"LBN"] = L"Lebanon";
		countries_code[L"LSO"] = L"Lesotho";
		countries_code[L"LBR"] = L"Liberia";
		countries_code[L"LBY"] = L"Libya";
		countries_code[L"LIE"] = L"Liechtenstein";
		countries_code[L"LTU"] = L"Lithuania";
		countries_code[L"LUX"] = L"Luxembourg";
		countries_code[L"MAC"] = L"Macao";
		countries_code[L"MDG"] = L"Madagascar";
		countries_code[L"MWI"] = L"Malawi";
		countries_code[L"MYS"] = L"Malaysia";
		countries_code[L"MDV"] = L"Maldives";
		countries_code[L"MLI"] = L"Mali";
		countries_code[L"MLT"] = L"Malta";
		countries_code[L"MHL"] = L"Marshall Islands";
		countries_code[L"MTQ"] = L"Martinique";
		countries_code[L"MRT"] = L"Mauritania";
		countries_code[L"MUS"] = L"Mauritius";
		countries_code[L"MYT"] = L"Mayotte";
		countries_code[L"MEX"] = L"Mexico";
		countries_code[L"FSM"] = L"Federated States of Micronesia";
		countries_code[L"MDA"] = L"Republic of Moldova";
		countries_code[L"MCO"] = L"Monaco";
		countries_code[L"MNG"] = L"Mongolia";
		countries_code[L"MNE"] = L"Montenegro";
		countries_code[L"MSR"] = L"Montserrat";
		countries_code[L"MAR"] = L"Morocco";
		countries_code[L"MOZ"] = L"Mozambique";
		countries_code[L"MMR"] = L"Myanmar";
		countries_code[L"NAM"] = L"Namibia";
		countries_code[L"NRU"] = L"Nauru";
		countries_code[L"NPL"] = L"Nepal";
		countries_code[L"NLD"] = L"Netherlands";
		countries_code[L"NCL"] = L"New Caledonia";
		countries_code[L"NZL"] = L"New Zealand";
		countries_code[L"NIC"] = L"Nicaragua";
		countries_code[L"NER"] = L"Niger";
		countries_code[L"NGA"] = L"Nigeria";
		countries_code[L"NIU"] = L"Niue";
		countries_code[L"NFK"] = L"Norfolk Island";
		countries_code[L"MKD"] = L"North Macedonia";
		countries_code[L"MNP"] = L"Northern Mariana Islands";
		countries_code[L"NOR"] = L"Norway";
		countries_code[L"OMN"] = L"Oman";
		countries_code[L"PAK"] = L"Pakistan";
		countries_code[L"PLW"] = L"Palau";
		countries_code[L"PSE"] = L"State of Palestine";
		countries_code[L"PAN"] = L"Panama";
		countries_code[L"PNG"] = L"Papua New Guinea";
		countries_code[L"PRY"] = L"Paraguay";
		countries_code[L"PER"] = L"Peru";
		countries_code[L"PHL"] = L"Philippines";
		countries_code[L"PCN"] = L"Pitcairn";
		countries_code[L"POL"] = L"Poland";
		countries_code[L"PRT"] = L"Portugal";
		countries_code[L"PRI"] = L"Puerto Rico";
		countries_code[L"QAT"] = L"Qatar";
		countries_code[L"REU"] = L"R\xE9union";				//Réunion
		countries_code[L"ROU"] = L"Romania";
		countries_code[L"RUS"] = L"Russian Federation";
		countries_code[L"RWA"] = L"Rwanda";
		countries_code[L"BLM"] = L"Saint Barth\xE9lemy";	//Saint Barthélemy
		countries_code[L"SHN"] = L"Saint Helena";
		countries_code[L"KNA"] = L"Saint Kitts and Nevis";
		countries_code[L"LCA"] = L"Saint Lucia";
		countries_code[L"MAF"] = L"Saint Martin (French)";
		countries_code[L"SPM"] = L"Saint Pierre and Miquelon";
		countries_code[L"VCT"] = L"Saint Vincent and the Grenadines";
		countries_code[L"WSM"] = L"Samoa";
		countries_code[L"SMR"] = L"San Marino";
		countries_code[L"STP"] = L"Sao Tome and Principe";
		countries_code[L"SAU"] = L"Saudi Arabia";
		countries_code[L"SEN"] = L"Senegal";
		countries_code[L"SRB"] = L"Serbia";
		countries_code[L"SYC"] = L"Seychelles";
		countries_code[L"SLE"] = L"Sierra Leone";
		countries_code[L"SGP"] = L"Singapore";
		countries_code[L"SXM"] = L"Sint Maarten (Dutch)";
		countries_code[L"SVK"] = L"Slovakia";
		countries_code[L"SVN"] = L"Slovenia";
		countries_code[L"SLB"] = L"Solomon Islands";
		countries_code[L"SOM"] = L"Somalia";
		countries_code[L"ZAF"] = L"South Africa";
		countries_code[L"SGS"] = L"South Georgia and the South Sandwich Islands";
		countries_code[L"SSD"] = L"South Sudan";
		countries_code[L"ESP"] = L"Spain";
		countries_code[L"LKA"] = L"Sri Lanka";
		countries_code[L"SDN"] = L"Sudan";
		countries_code[L"SUR"] = L"Suriname";
		countries_code[L"SJM"] = L"Svalbard and Jan Mayen Islands";
		countries_code[L"SWE"] = L"Sweden";
		countries_code[L"CHE"] = L"Switzerland";
		countries_code[L"SYR"] = L"Syrian Arab Republic";
		countries_code[L"TWN"] = L"Taiwan, Republic of China";
		countries_code[L"TJK"] = L"Tajikistan";
		countries_code[L"TZA"] = L"United Republic of Tanzania";
		countries_code[L"THA"] = L"Thailand";
		countries_code[L"TLS"] = L"Timor-Leste";
		countries_code[L"TGO"] = L"Togo";
		countries_code[L"TKL"] = L"Tokelau";
		countries_code[L"TON"] = L"Tonga";
		countries_code[L"TTO"] = L"Trinidad and Tobago";
		countries_code[L"TUN"] = L"Tunisia";
		countries_code[L"TUR"] = L"Turkey";
		countries_code[L"TKM"] = L"Turkmenistan";
		countries_code[L"TCA"] = L"Turks and Caicos Islands";
		countries_code[L"TUV"] = L"Tuvalu";
		countries_code[L"UGA"] = L"Uganda";
		countries_code[L"UKR"] = L"Ukraine";
		countries_code[L"ARE"] = L"United Arab Emirates";
		countries_code[L"GBR"] = L"United Kingdom";
		countries_code[L"UMI"] = L"United States Minor Outlying Islands";
		countries_code[L"USA"] = L"United States of America";
		countries_code[L"URY"] = L"Uruguay";
		countries_code[L"UZB"] = L"Uzbekistan";
		countries_code[L"VUT"] = L"Vanuatu";
		countries_code[L"VEN"] = L"Venezuela";
		countries_code[L"VNM"] = L"Viet Nam";
		countries_code[L"VGB"] = L"Virgin Islands (British)";
		countries_code[L"VIR"] = L"Virgin Islands (U.S.)";
		countries_code[L"WLF"] = L"Wallis and Futuna Islands";
		countries_code[L"ESH"] = L"Western Sahara";
		countries_code[L"YEM"] = L"Yemen";
		countries_code[L"ZMB"] = L"Zambia";
		countries_code[L"ZWE"] = L"Zimbabwe";

		countries_code[L"GBD"] = L"British Overseas Territories Citizen";
		countries_code[L"GBN"] = L"British National (Overseas)";
		countries_code[L"GBO"] = L"British Overseas Citizen";
		countries_code[L"GBP"] = L"British Protected Person";
		countries_code[L"GBS"] = L"British Subject";

		countries_code[L"UNO"] = L"United Nations";
		countries_code[L"UNA"] = L"United Nations";
		countries_code[L"UNK"] = L"Kosovo (United Nations)";

		countries_code[L"XBA"] = L"African Development Bank";
		countries_code[L"XIM"] = L"African Export-Import Bank";
		countries_code[L"XCC"] = L"Caribbean Community";
		countries_code[L"XCE"] = L"Council of Europe";
		countries_code[L"XCO"] = L"Common Market for Eastern and Southern Africa";
		countries_code[L"XEC"] = L"Economic Community of West African States";
		countries_code[L"XPO"] = L"Interpol";
		countries_code[L"XOM"] = L"Sovereign Military Order of Malta";
		countries_code[L"XDC"] = L"Southern African Development Community";

		countries_code[L"XXA"] = L"Stateless";
		countries_code[L"XXB"] = L"Refugee";
		countries_code[L"XXC"] = L"Refugee";
		countries_code[L"XXX"] = L"Unspecified";

		countries_code[L"EUE"] = L"European Union";
		countries_code[L"RKS"] = L"Republic of Kosovo";
		countries_code[L"XCT"] = L"North Cyprus";

		countries_code[L"ANT"] = L"Netherlands Antilles";
		countries_code[L"NTZ"] = L"Neutral Zone";
		countries_code[L"SCG"] = L"Serbia and Montenegro";
		countries_code[L"YUG"] = L"Yugoslavia";
		countries_code[L"FXX"] = L"France, Metropolitan";

	}

	static std::map<std::wstring, std::wstring> & Codes(void) {
		static std::map<std::wstring, std::wstring> countries_code;
		return countries_code;
	}
};

} // namespace Extension
} // namespace Pr22
//------------------------------------------------------------------------------
#endif //PR22_COUNTRYCODE_INCL
