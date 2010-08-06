
# QT4_WRAP_TS(lupdate_outputs lrelease_outputs prefix lang1 lang2 ... )
MACRO (QT4_WRAP_TS lupdate_outputs lrelease_outputs prefix)
    FOREACH (it ${ARGN})
      SET(tsfile ${CMAKE_CURRENT_SOURCE_DIR}/translations/${prefix}_${it}.ts)
#      SET(tsfile ${CMAKE_CURRENT_BINARY_DIR}/translations/${prefix}_${it}.ts)
      ADD_CUSTOM_COMMAND(OUTPUT ${tsfile}
        COMMAND ${QT_LUPDATE_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}  -recursive -ts -locations none ${tsfile}
      )

      SET(qmfile ${CMAKE_CURRENT_BINARY_DIR}/${prefix}_${it}.qm)
      ADD_CUSTOM_COMMAND(OUTPUT ${qmfile}
        COMMAND ${QT_LRELEASE_EXECUTABLE} ${tsfile} -qm ${qmfile}
      )

      SET(${lupdate_outputs} ${${lupdate_outputs}} ${tsfile})
      SET(${lrelease_outputs} ${${lrelease_outputs}} ${qmfile})
    ENDFOREACH(it)
ENDMACRO (QT4_WRAP_TS)


