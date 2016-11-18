#ifndef REEDSOLOMON_HPP
#define REEDSOLOMON_HPP
#include <QtCore/QObject>
#include <QtCore/QVariant>

class Logs;

/*! \ingroup UTILS
 * \class ReedSolomon
 * \brief ReedSolomon encode and decode
 * \details
 * This is a Reed-Solomon class to encode and decode Reed-Solomon
 * codes. This is adapted from the C code of  Simon Rockliff,
 * 26th June 1991.
 *
 * This program is an encoder/decoder for Reed-Solomon codes.
 * Encoding is in systematic form, decoding via the Berlekamp
 * iterative algorithm.
 *
 * In the present form, the constants mm, nn, tt, and kk=nn-2tt
 * must be specified (the double letters are used simply to avoid
 * clashes with other n,k,t used in other programs into which this
 * was incorporated!). Also, the irreducible polynomial used to
 * generate GF(2**mm) must also be entered -- these can be found
 * in Lin and Costello, and also Clark and Cain.
 *
 * The representation of the elements of GF(2**m) is either in
 * index form, where the number is the power of the primitive
 * element alpha, which is convenient for multiplication (add
 * the powers modulo 2**m-1) or in polynomial form, where the
 * bits represent the coefficients of the polynomial
 * representation of the number, which is the most convenient
 * form for addition. The two forms are swapped between via
 * lookup tables.
 *
 * This leads to fairly messy looking expressions, but
 * unfortunately, there is no easy alternative when working with
 * Galois arithmetic.
 *
 * The code is not written in the most elegant way, but to the
 * best of my knowledge, (no absolute guarantees!), it works.
 *
 * However, when including it into a simulation program, you may
 * want to do some conversion of global variables (used here
 * because I am lazy!) to local variables where appropriate, and
 * passing parameters (eg array addresses) to the functions may
 * be a sensible move to reduce the number of global variables
 * and thus decrease the chance of a bug being introduced.
 *
 * This program does not handle erasures at present, but should
 * not be hard to adapt to do this, as it is just an adjustment
 * to the Berlekamp-Massey algorithm. It also does not attempt
 * to decode past the BCH bound -- see Blahut "Theory and
 * practice of error control codes" for how to do this.
 *
 * Simon Rockliff, University of Adelaide   21/9/89
 *
 * 26/6/91 Slight modifications to remove a compiler dependent
 * bug which hadn't previously surfaced. A few extra comments
 * added for clarity. Appears to all work fine, ready for
 * posting to net!
 *                  Notice
 *                 --------
 * This program may be freely modified and/or given to whoever
 * wants it. A condition of such distribution is that the
 * author's contribution be acknowledged by his name being left
 * in the comments heading the program, however no responsibility
 * is accepted for any financial or other loss which may result
 * from some unforseen errors or malfunctioning of the program
 * during use.
 *          Simon Rockliff, 26th June 1991
 *
 *
 * Adapted from the Visual Basic Code of Rick Muething, KN6KB
 * when used in the application known as WinMor TNC...AD5XJ  */
class ReedSolomon8 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 m_maxcorr READ getMaxCorrections WRITE setMaxCorrections)

public:
    explicit ReedSolomon8(QObject *parent);
    virtual ~ReedSolomon8();

    // property sets and gets
    void setMaxCorrections(qint32 max);

    quint32 getMaxCorrections()              { return m_maxcorr; }
    // end property sets and gets


private:
    //  property vars    //
    quint32 m_maxcorr;
    //                   //

    class RSPrivate;
    RSPrivate *d;

    /*! \brief Subroutine to generate the GF
     * \details
     * Generate GF(2**mm) from the irreducible
     * polynomial p(X) in pp[0]..pp[mm]
     * lookup tables:<br />
     * &nbsp;&nbsp;&nbsp;index->polynomial form
     * alpha_to[] contains j = alpha**i;
     * polynomial form -> index form  index_of[j=alpha**i] = i
     * alpha=2 is the primitive element of GF(2**mm)  */
    void generate_gf();

    /*! \brief Subroutine to generate the polynomial
     * \details
     * Obtain the generator polynomial of the tt-error
     * correcting, length nn=(2**mm -1) Reed Solomon
     * code  from the product of (X+alpha**i), i=1..2*tt  */
    void gen_poly();

    /*! \brief Subroutine to Encode Reed Solomon
     * \details
     * Take the string of symbols in data[i],
     * i=0..(k-1) and encode systematically to
     * produce 2*tt parity symbols in bb[0]..bb[2*tt-1]
     * data[] is input and bb[] is output in polynomial form.
     * Encoding is done by using a feedback shift register
     * with appropriate connections specified by the
     * elements of gg[], which was generated above.
     *
     * Codeword is   c(X) = data(X)*X**(nn-kk)+ b(X).  */
    void encode_rs();

    /*! \brief Subroutine to Decode RS
     *  \details
     * Assume we have received bits grouped into
     * mm-bit symbols in recd[i], i=0..(nn-1), and
     * recd[i] is index form (ie as powers of alpha).
     * We first compute the 2*tt syndromes by
     * substituting alpha**i into rec(X) and
     * evaluating, storing the syndromes in s[i],
     * i=1..2tt (leave s[0] zero).
     *
     * Then we use the Berlekamp iteration to find
     * the error location polynomial elp[i].
     * If the degree of the elp is >tt, we cannot
     * correct all the errors and hence just put
     * out the information symbols uncorrected. If
     * the degree of elp is <=tt, we substitute
     * alpha**i , i=1..n into the elp to get the roots,
     * hence the inverse roots, the error location numbers.
     * If the number of errors located does not equal
     * the degree of the elp, we have more than tt errors
     * and cannot correct them.  Otherwise, we then solve
     * for the error value at the error location and
     * correct the error.  The procedure is that found in
     * Lin and Costello. For the cases where the number of
     * errors is known to be too large to correct, the
     * information symbols as received are output (the
     * advantage of systematic encoding is that hopefully
     * some of the information symbols will be okay and that
     * if we are in luck, the errors are in the parity part
     * of the transmitted codeword). Of course, these
     * insoluble cases can be returned as error flags to
     * the calling routine if desired.
     *
     * Compute the error location polynomial via the
     * Berlekamp iterative algorithm, following the
     * terminology of Lin and Costello:<br />
     * &nbsp;&nbsp;&nbsp;dis[u] is the 'mu'th discrepancy,<br />
     * &nbsp;&nbsp;&nbsp;where u='mu'+1 and <br />
     * &nbsp;&nbsp;&nbsp;'mu' (the Greek letter!)<br />
     * &nbsp;&nbsp;&nbsp;is the step number ranging from -1 to 2*tt
     * (see L&C),<br />
     * &nbsp;&nbsp;&nbsp;l[u] is the degree of the elp at that
     * step, and <br />
     * &nbsp;&nbsp;&nbsp;u_l[u] is the difference between the
     * step number and the degree of the elp.     */
    void decode_rs();
};

#endif // REEDSOLOMON_HPP
