#include "GeoLite2PP.h"
#include "GeoLite2PP_error_category.h"

const char *city[] = {"city", NULL};
const char *country[] = {"country", NULL};
const char *prov[] = {"prov", NULL};
const char *isp[] = {"isp", NULL};

static std::string emptystr;

GeoLite2PP::DB::~DB(void) {
  MMDB_close(&mmdb);
  return;
}

GeoLite2PP::DB::DB(const std::string &database_filename) {
  const int status = MMDB_open(database_filename.c_str(), MMDB_MODE_MMAP, &mmdb);
  if (status != MMDB_SUCCESS) {
    const ErrorCategory &cat(get_error_category());
    const std::error_code ec(status, cat);
    const std::string msg = "Failed to open the MMDB database \"" + database_filename + "\"";

    /** @throw std::system_error if the database file cannot be opened.
    * @see @ref GeoLite2PP::MMDBStatus
    * @see @ref GeoLite2PP::ErrorCategory
    */
    throw std::system_error(ec, msg);
  }

  return;
}

MMDB_lookup_result_s GeoLite2PP::DB::lookup_raw(const char *ip_address) {
  int gai_error, mmdb_error;

  return MMDB_lookup_string(&mmdb, ip_address, &gai_error, &mmdb_error);
}

std::string &GeoLite2PP::DB::get_field(const char *ip_address, const char **v) {
  MMDB_lookup_result_s lookup = lookup_raw(ip_address);
  if (lookup.found_entry)
    return get_field(&lookup, v);
  else
    return emptystr;
}

void GeoLite2PP::DB::get_field(const char *ip_address) {
  city_ = isp_ = prov_ = 0;

  MMDB_lookup_result_s lookup = lookup_raw(ip_address);
  if (lookup.found_entry)
    get_field(&lookup);
}

std::string &GeoLite2PP::DB::get_field(MMDB_lookup_result_s *lookup, const char **v) {
  if (lookup) {
    MMDB_entry_s *entry = &lookup->entry;
    MMDB_entry_data_s result;
    MMDB_aget_value(entry, &result, v);

    if (result.has_data) {
      switch (result.type) {
        case MMDB_DATA_TYPE_UTF8_STRING: str_ = std::string(result.utf8_string, result.data_size);
          break;
        case MMDB_DATA_TYPE_DOUBLE: str_ = std::to_string(result.double_value);
          break;
        case MMDB_DATA_TYPE_UINT16: str_ = std::to_string(result.uint16);
          break;
        case MMDB_DATA_TYPE_UINT32: str_ = std::to_string(result.uint32);
          break;
        case MMDB_DATA_TYPE_INT32: str_ = std::to_string(result.uint32);
          break;
        case MMDB_DATA_TYPE_UINT64: str_ = std::to_string(result.uint64);
          break;
        case MMDB_DATA_TYPE_FLOAT: str_ = std::to_string(result.float_value);
          break;
        case MMDB_DATA_TYPE_BOOLEAN: str_ = (result.boolean ? "true" : "false");
          break;
        default:    /* data type not supported for this "quick" retrieval */                            break;
      }
    }
    return str_;
  }
  return emptystr;
}

void GeoLite2PP::DB::get_field(MMDB_lookup_result_s *lookup) {
  if (lookup) {
    MMDB_entry_s *entry = &lookup->entry;
    MMDB_entry_data_s entry_data;

    int status = MMDB_get_value(entry, &entry_data, "country", "iso_code", NULL);
    if(MMDB_SUCCESS == status && entry_data.has_data && entry_data.type == MMDB_DATA_TYPE_UTF8_STRING)
    {
      country_ = std::string(entry_data.utf8_string, entry_data.data_size);
      //country_(entry_data.utf8_string);
    }
  }
}

void GeoLite2PP::DB::get_geoinfo(const char *ip_address, std::string &country) {
  get_field(ip_address);
  country = country_;
}
