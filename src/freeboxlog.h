#pragma once

#include <QLoggingCategory>

namespace Freebox {

// Logging category for the whole plugin. Enable verbose traces with e.g.
//   QT_LOGGING_RULES="freebox.debug=true"
Q_DECLARE_LOGGING_CATEGORY(fbxLog)

} // namespace Freebox
