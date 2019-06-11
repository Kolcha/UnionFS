#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

struct config_event_handlers {
  /**
   * Section start handler
   *
   * Called on each section start. Arguments are section name and
   * user data pointer passed to read_config().
   *
   * The return value will passed as a parameter to the section_end() and
   * option_found() methods.
   */
  void* (*section_start) (const char*, void*);
  /**
   * Section end handler
   *
   * Called on each section end. Arguments are pointer to section-specific
   * data returned from section_start() and user data pointer passed to
   * read_config().
   */
  void (*section_end) (void*, void*);
  /**
   * Option found handler
   *
   * Called on each found key-value pair. Arguments are pointer to
   * section-specific data returned from section_start(), parsed key and
   * its value, and user data pointer passed to read_config().
   */
  void (*option_found) (void*, const char*, const char*, void*);
};


void config_read_file(const char* filename,
                      struct config_event_handlers* handlers,
                      void* user_data);

#endif // CONFIG_FILE_H
